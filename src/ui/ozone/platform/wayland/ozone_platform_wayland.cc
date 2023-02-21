// Copyright 2023 The Chromium Authors and Alex313031
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/ozone/platform/wayland/ozone_platform_wayland.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <components/exo/wayland/protocol/aura-shell-client-protocol.h>

#include "base/bind.h"
#include "base/command_line.h"
#include "base/memory/ptr_util.h"
#include "base/message_loop/message_pump_type.h"
#include "base/no_destructor.h"
#include "base/task/single_thread_task_runner.h"
#include "build/build_config.h"
#include "ui/base/buildflags.h"
#include "ui/base/cursor/cursor_factory.h"
#include "ui/base/dragdrop/os_exchange_data_provider_factory_ozone.h"
#include "ui/base/ime/linux/input_method_auralinux.h"
#include "ui/base/ime/linux/linux_input_method_context_factory.h"
#include "ui/base/ui_base_features.h"
#include "ui/display/display_switches.h"
#include "ui/events/devices/device_data_manager.h"
#include "ui/events/event.h"
#include "ui/events/ozone/layout/keyboard_layout_engine_manager.h"
#include "ui/gfx/buffer_format_util.h"
#include "ui/gfx/linux/client_native_pixmap_dmabuf.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/ozone/common/features.h"
#include "ui/ozone/platform/wayland/common/wayland_util.h"
#include "ui/ozone/platform/wayland/gpu/wayland_buffer_manager_gpu.h"
#include "ui/ozone/platform/wayland/gpu/wayland_gl_egl_utility.h"
#include "ui/ozone/platform/wayland/gpu/wayland_overlay_manager.h"
#include "ui/ozone/platform/wayland/gpu/wayland_surface_factory.h"
#include "ui/ozone/platform/wayland/host/wayland_buffer_manager_connector.h"
#include "ui/ozone/platform/wayland/host/wayland_buffer_manager_host.h"
#include "ui/ozone/platform/wayland/host/wayland_connection.h"
#include "ui/ozone/platform/wayland/host/wayland_event_source.h"
#include "ui/ozone/platform/wayland/host/wayland_exchange_data_provider.h"
#include "ui/ozone/platform/wayland/host/wayland_input_controller.h"
#include "ui/ozone/platform/wayland/host/wayland_input_method_context.h"
#include "ui/ozone/platform/wayland/host/wayland_menu_utils.h"
#include "ui/ozone/platform/wayland/host/wayland_output_manager.h"
#include "ui/ozone/platform/wayland/host/wayland_window.h"
#include "ui/ozone/platform/wayland/host/wayland_zaura_shell.h"
#include "ui/ozone/platform/wayland/wayland_utils.h"
#include "ui/ozone/public/gpu_platform_support_host.h"
#include "ui/ozone/public/ozone_platform.h"
#include "ui/ozone/public/platform_menu_utils.h"
#include "ui/ozone/public/system_input_injector.h"
#include "ui/platform_window/platform_window_init_properties.h"

#if BUILDFLAG(USE_XKBCOMMON)
#include "ui/events/ozone/layout/xkb/xkb_evdev_codes.h"
#include "ui/events/ozone/layout/xkb/xkb_keyboard_layout_engine.h"
#else
#include "ui/events/ozone/layout/stub/stub_keyboard_layout_engine.h"
#endif

#if BUILDFLAG(IS_CHROMEOS)
#include "ui/ozone/common/bitmap_cursor_factory.h"
#else
#include "ui/ozone/platform/wayland/host/wayland_cursor_factory.h"
#endif

#if BUILDFLAG(IS_LINUX)
#include "ui/ozone/platform/wayland/host/linux_ui_delegate_wayland.h"
#endif

#if defined(WAYLAND_GBM)
#include "ui/ozone/platform/wayland/gpu/drm_render_node_path_finder.h"
#endif

namespace ui {

namespace {

class OzonePlatformWayland : public OzonePlatform,
                             public OSExchangeDataProviderFactoryOzone {
 public:
  OzonePlatformWayland()
      : old_synthesize_key_repeat_enabled_(
            KeyEvent::IsSynthesizeKeyRepeatEnabled()) {
    // Forcing the device scale factor on Wayland is not fully/well supported
    // and is provided for test purposes only.
    // See https://crbug.com/1241546
    base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
    if (command_line->HasSwitch(switches::kForceDeviceScaleFactor)) {
      LOG(WARNING) << "--" << switches::kForceDeviceScaleFactor
                   << " on Wayland is TEST ONLY.  Use it at your own risk.";
    }

    // Disable key-repeat flag synthesizing. On Wayland, key repeat events are
    // generated inside Chrome, and the flag is properly set.
    // See also WaylandEventSource.
    KeyEvent::SetSynthesizeKeyRepeatEnabled(false);

    OSExchangeDataProviderFactoryOzone::SetInstance(this);
  }

  OzonePlatformWayland(const OzonePlatformWayland&) = delete;
  OzonePlatformWayland& operator=(const OzonePlatformWayland&) = delete;

  ~OzonePlatformWayland() override {
    KeyEvent::SetSynthesizeKeyRepeatEnabled(old_synthesize_key_repeat_enabled_);
    GetInputMethodContextFactoryForOzone() = LinuxInputMethodContextFactory();
  }

  // OzonePlatform
  SurfaceFactoryOzone* GetSurfaceFactoryOzone() override {
    return surface_factory_.get();
  }

  OverlayManagerOzone* GetOverlayManager() override {
    return overlay_manager_.get();
  }

  CursorFactory* GetCursorFactory() override { return cursor_factory_.get(); }

  InputController* GetInputController() override {
    return input_controller_.get();
  }

  GpuPlatformSupportHost* GetGpuPlatformSupportHost() override {
    return buffer_manager_connector_ ? buffer_manager_connector_.get()
                                     : gpu_platform_support_host_.get();
  }

  std::unique_ptr<SystemInputInjector> CreateSystemInputInjector() override {
    return nullptr;
  }

  std::unique_ptr<PlatformWindow> CreatePlatformWindow(
      PlatformWindowDelegate* delegate,
      PlatformWindowInitProperties properties) override {
    return WaylandWindow::Create(delegate, connection_.get(),
                                 std::move(properties));
  }

  std::unique_ptr<display::NativeDisplayDelegate> CreateNativeDisplayDelegate()
      override {
    return nullptr;
  }

  std::unique_ptr<PlatformScreen> CreateScreen() override {
    // The WaylandConnection and the WaylandOutputManager must be created
    // before PlatformScreen.
    DCHECK(connection_ && connection_->wayland_output_manager());
    return connection_->wayland_output_manager()->CreateWaylandScreen();
  }

  void InitScreen(PlatformScreen* screen) override {
    DCHECK(connection_ && connection_->wayland_output_manager());
    // InitScreen is always called with the same screen that CreateScreen
    // hands back, so it is safe to cast here.
    connection_->wayland_output_manager()->InitWaylandScreen(
        static_cast<WaylandScreen*>(screen));
  }

  PlatformClipboard* GetPlatformClipboard() override {
    DCHECK(connection_);
    return connection_->clipboard();
  }

  PlatformGLEGLUtility* GetPlatformGLEGLUtility() override {
    if (!gl_egl_utility_)
      gl_egl_utility_ = std::make_unique<WaylandGLEGLUtility>();
    return gl_egl_utility_.get();
  }

  std::unique_ptr<InputMethod> CreateInputMethod(
      ImeKeyEventDispatcher* ime_key_event_dispatcher,
      gfx::AcceleratedWidget widget) override {
    return std::make_unique<InputMethodAuraLinux>(ime_key_event_dispatcher);
  }

  PlatformMenuUtils* GetPlatformMenuUtils() override {
    return menu_utils_.get();
  }

  WaylandUtils* GetPlatformUtils() override { return wayland_utils_.get(); }

  bool IsNativePixmapConfigSupported(gfx::BufferFormat format,
                                     gfx::BufferUsage usage) const override {
#if defined(WAYLAND_GBM)
    // If there is no drm render node device available, native pixmaps are not
    // supported.
    if (path_finder_.GetDrmRenderNodePath().empty())
      return false;

    if (supported_buffer_formats_.find(format) ==
        supported_buffer_formats_.end()) {
      return false;
    }

    return gfx::ClientNativePixmapDmaBuf::IsConfigurationSupported(format,
                                                                   usage);
#else
    return false;
#endif
  }

  bool ShouldUseCustomFrame() override {
    return connection_->xdg_decoration_manager_v1() == nullptr;
  }

  bool InitializeUI(const InitParams& args) override {
    if (ShouldFailInitializeUIForTest()) {
      LOG(ERROR) << "Failing for test";
      return false;
    }
    // Initialize DeviceDataManager early as devices are set during
    // WaylandConnection::Initialize().
    DeviceDataManager::CreateInstance();
#if BUILDFLAG(USE_XKBCOMMON)
    keyboard_layout_engine_ =
        std::make_unique<XkbKeyboardLayoutEngine>(xkb_evdev_code_converter_);
#else
    keyboard_layout_engine_ = std::make_unique<StubKeyboardLayoutEngine>();
#endif
    KeyboardLayoutEngineManager::SetKeyboardLayoutEngine(
        keyboard_layout_engine_.get());
    connection_ = std::make_unique<WaylandConnection>();
    if (!connection_->Initialize()) {
      LOG(ERROR) << "Failed to initialize Wayland platform";
      return false;
    }

    buffer_manager_connector_ = std::make_unique<WaylandBufferManagerConnector>(
        connection_->buffer_manager_host());
#if BUILDFLAG(IS_CHROMEOS)
    cursor_factory_ = std::make_unique<BitmapCursorFactory>();
#else
    cursor_factory_ = std::make_unique<WaylandCursorFactory>(connection_.get());
#endif
    input_controller_ = CreateWaylandInputController(connection_.get());
    gpu_platform_support_host_.reset(CreateStubGpuPlatformSupportHost());

    supported_buffer_formats_ =
        connection_->buffer_manager_host()->GetSupportedBufferFormats();
#if BUILDFLAG(IS_LINUX)
    linux_ui_delegate_ =
        std::make_unique<LinuxUiDelegateWayland>(connection_.get());
#endif

    menu_utils_ = std::make_unique<WaylandMenuUtils>(connection_.get());
    wayland_utils_ = std::make_unique<WaylandUtils>(connection_.get());

    GetInputMethodContextFactoryForOzone() = base::BindRepeating(
        [](WaylandConnection* connection,
           WaylandKeyboard::Delegate* key_delegate,
           LinuxInputMethodContextDelegate* ime_delegate)
            -> std::unique_ptr<LinuxInputMethodContext> {
          return std::make_unique<WaylandInputMethodContext>(
              connection, key_delegate, ime_delegate);
        },
        base::Unretained(connection_.get()),
        base::Unretained(connection_->event_source()));

    return true;
  }

  void InitializeGPU(const InitParams& args) override {
    base::FilePath drm_node_path;
#if defined(WAYLAND_GBM)
    drm_node_path = path_finder_.GetDrmRenderNodePath();
    if (drm_node_path.empty())
      LOG(WARNING) << "Failed to find drm render node path.";
#endif
    buffer_manager_ = std::make_unique<WaylandBufferManagerGpu>(drm_node_path);
    surface_factory_ = std::make_unique<WaylandSurfaceFactory>(
        connection_.get(), buffer_manager_.get());
    overlay_manager_ =
        std::make_unique<WaylandOverlayManager>(buffer_manager_.get());
  }

  const PlatformProperties& GetPlatformProperties() override {
    static base::NoDestructor<OzonePlatform::PlatformProperties> properties;
    static bool initialised = false;
    if (!initialised) {
      // Server-side decorations on Wayland require support of xdg-decoration or
      // some other protocol extensions specific for the particular environment.
      // Whether the environment has any support only gets known at run time, so
      // we use the custom frame by default.  If there is support, the user will
      // be able to enable the system frame.
      properties->custom_frame_pref_default = true;

      // Wayland uses sub-surfaces to show tooltips, and sub-surfaces must be
      // bound to their root surfaces always, but finding the correct root
      // surface at the moment of creating the tooltip is not always possible
      // due to how Wayland handles focus and activation.
      // Therefore, the platform should be given a hint at the moment when the
      // surface is initialised, where it is known for sure which root surface
      // shows the tooltip.
      properties->set_parent_for_non_top_level_windows = true;
      properties->app_modal_dialogs_use_event_blocker = true;

      // Xdg/Wl shell protocol does not disallow clients to manipulate global
      // screen coordinates, instead only surface-local ones are supported.
      // Non-toplevel surfaces, for example, must be positioned relative to
      // their parents. As for toplevel surfaces, clients simply don't know
      // their position on screens and always assume they are located at some
      // arbitrary position.
      properties->supports_global_screen_coordinates =
          features::IsWaylandScreenCoordinatesEnabled();
          
      // Let the media know this platform supports va-api.
      properties->supports_vaapi = true;

      initialised = true;
    }

    return *properties;
  }

  const PlatformRuntimeProperties& GetPlatformRuntimeProperties() override {
    using SupportsSsdForTest =
        OzonePlatform::PlatformRuntimeProperties::SupportsSsdForTest;
    const auto& override_supports_ssd_for_test = OzonePlatform::
        PlatformRuntimeProperties::override_supports_ssd_for_test;

    static OzonePlatform::PlatformRuntimeProperties properties;
    if (connection_) {
      DCHECK(has_initialized_ui());
      // These properties are set when GetPlatformRuntimeProperties is called on
      // the browser process side.
      properties.supports_server_side_window_decorations =
          (connection_->xdg_decoration_manager_v1() != nullptr &&
          override_supports_ssd_for_test == SupportsSsdForTest::kNotSet) ||
          override_supports_ssd_for_test == SupportsSsdForTest::kYes;
      properties.supports_overlays =
          ui::IsWaylandOverlayDelegationEnabled() && connection_->viewporter();
      properties.supports_non_backed_solid_color_buffers =
          ui::IsWaylandOverlayDelegationEnabled() &&
          connection_->buffer_manager_host()
              ->SupportsNonBackedSolidColorBuffers();
      // Primary planes can be transluscent due to underlay strategy. As a
      // result Wayland server draws contents occluded by an accelerated widget.
      // To prevent this, an opaque background image is stacked below the
      // accelerated widget to occlude contents below.
      properties.needs_background_image =
          ui::IsWaylandOverlayDelegationEnabled() && connection_->viewporter();
      if (connection_->zaura_shell()) {
        properties.supports_activation =
            zaura_shell_get_version(connection_->zaura_shell()->wl_object()) >=
            ZAURA_TOPLEVEL_ACTIVATE_SINCE_VERSION;
        properties.supports_tooltip =
            (wl::get_version_of_object(
                 connection_->zaura_shell()->wl_object()) >=
             ZAURA_SURFACE_SHOW_TOOLTIP_SINCE_VERSION) &&
            connection_->zaura_shell()->HasBugFix(1400226);
      }

      if (surface_factory_) {
        DCHECK(has_initialized_gpu());
        properties.supports_native_pixmaps =
            surface_factory_->SupportsNativePixmaps();
      }
    } else if (buffer_manager_) {
      DCHECK(has_initialized_gpu());
      // These properties are set when the GetPlatformRuntimeProperties is
      // called on the gpu process side.
      properties.supports_non_backed_solid_color_buffers =
          ui::IsWaylandOverlayDelegationEnabled() &&
          buffer_manager_->supports_non_backed_solid_color_buffers();
      // See the comment above.
      properties.needs_background_image =
          ui::IsWaylandOverlayDelegationEnabled() &&
          buffer_manager_->supports_viewporter();
      properties.supports_native_pixmaps =
          surface_factory_->SupportsNativePixmaps();
      properties.supports_clip_rect = buffer_manager_->supports_clip_rect();
    }
    return properties;
  }

  void AddInterfaces(mojo::BinderMap* binders) override {
    // It's preferred to reuse the same task runner where the
    // WaylandBufferManagerGpu has been created. However, when tests are
    // executed, the task runner might not have been set at that time. Thus, use
    // the current one. See the comment in WaylandBufferManagerGpu why it takes
    // a task runner.
    //
    // Please note this call happens on the gpu.
    auto gpu_task_runner = buffer_manager_->gpu_thread_runner();
    if (!gpu_task_runner)
      gpu_task_runner = base::SingleThreadTaskRunner::GetCurrentDefault();

    binders->Add<ozone::mojom::WaylandBufferManagerGpu>(
        base::BindRepeating(
            &OzonePlatformWayland::CreateWaylandBufferManagerGpuBinding,
            base::Unretained(this)),
        gpu_task_runner);
  }

  void CreateWaylandBufferManagerGpuBinding(
      mojo::PendingReceiver<ozone::mojom::WaylandBufferManagerGpu> receiver) {
    buffer_manager_->AddBindingWaylandBufferManagerGpu(std::move(receiver));
  }

  void PostCreateMainMessageLoop(
      base::OnceCallback<void()> shutdown_cb,
      scoped_refptr<base::SingleThreadTaskRunner>) override {
    DCHECK(connection_);
    connection_->SetShutdownCb(std::move(shutdown_cb));
  }

  // OSExchangeDataProviderFactoryOzone:
  std::unique_ptr<OSExchangeDataProvider> CreateProvider() override {
    return std::make_unique<WaylandExchangeDataProvider>();
  }

 private:
  // Keeps the old value of KeyEvent::IsSynthesizeKeyRepeatEnabled(), to
  // restore it on destruction.
  const bool old_synthesize_key_repeat_enabled_;

#if BUILDFLAG(USE_XKBCOMMON)
  XkbEvdevCodes xkb_evdev_code_converter_;
#endif

  std::unique_ptr<KeyboardLayoutEngine> keyboard_layout_engine_;
  std::unique_ptr<WaylandConnection> connection_;
  std::unique_ptr<WaylandSurfaceFactory> surface_factory_;
  std::unique_ptr<CursorFactory> cursor_factory_;
  std::unique_ptr<InputController> input_controller_;
  std::unique_ptr<GpuPlatformSupportHost> gpu_platform_support_host_;
  std::unique_ptr<WaylandBufferManagerConnector> buffer_manager_connector_;
  std::unique_ptr<WaylandMenuUtils> menu_utils_;
  std::unique_ptr<WaylandUtils> wayland_utils_;

  // Objects, which solely live in the GPU process.
  std::unique_ptr<WaylandBufferManagerGpu> buffer_manager_;
  std::unique_ptr<WaylandOverlayManager> overlay_manager_;
  std::unique_ptr<WaylandGLEGLUtility> gl_egl_utility_;

  // Provides supported buffer formats for native gpu memory buffers
  // framework.
  wl::BufferFormatsWithModifiersMap supported_buffer_formats_;

#if defined(WAYLAND_GBM)
  // This is used both in the gpu and browser processes to find out if a drm
  // render node is available.
  DrmRenderNodePathFinder path_finder_;
#endif

#if BUILDFLAG(IS_LINUX)
  std::unique_ptr<LinuxUiDelegateWayland> linux_ui_delegate_;
#endif
};

}  // namespace

OzonePlatform* CreateOzonePlatformWayland() {
  return new OzonePlatformWayland;
}

}  // namespace ui
