const fs = require('fs');
const path = require('path');
const os = require('os');
const childProcess = require('child_process');
const process = require('process');

let genPatch = false;
let output = path.resolve(os.homedir(),'./Desktop');
let ffmpegRoot = path.resolve('./');

for (const [idx, argv] of process.argv.entries()) {
  switch (argv) {
    case '-r':
      ffmpegRoot = process.argv[idx + 1];
      if (typeof ffmpegRoot !== 'string' || !fs.existsSync(genPath('./ffmpeg_generated.gni'))) {
        console.error('Please provide the correct path of `src/third_party/ffmpeg`!');
        process.exit(1);
      }
      break;
    case '-o':
      output = process.argv[idx + 1];
      if (!fs.existsSync(output)) {
        fs.mkdirSync(output, {
          recursive: true
        });
      }
      break;
    case '-g':
      genPatch = true;
      break;
    default:
      break;
  }
}

const encodingConfig = { encoding: 'utf8' };
const patches = [
  {
    condition: 'if ((is_apple && ffmpeg_branding == "Chrome") || (is_win && ffmpeg_branding == "Chrome") || (use_linux_config && ffmpeg_branding == "Chrome") || (use_linux_config && ffmpeg_branding == "ChromeOS")) {',
    ffmpeg_c_sources : [
      'libavcodec/autorename_libavcodec_bswapdsp.c',
      'libavcodec/dovi_rpu.c',
      'libavcodec/dynamic_hdr_vivid.c',
      'libavcodec/hevc_cabac.c',
      'libavcodec/hevc_data.c',
      'libavcodec/hevc_filter.c',
      'libavcodec/hevc_mvs.c',
      'libavcodec/hevc_parse.c',
      'libavcodec/hevc_parser.c',
      'libavcodec/hevc_ps.c',
      'libavcodec/hevc_refs.c',
      'libavcodec/hevc_sei.c',
      'libavcodec/hevcdec.c',
      'libavcodec/hevcdsp.c',
      'libavcodec/hevcpred.c',
    ]
  },
  {
    condition: 'if ((is_apple && current_cpu == "x64" && ffmpeg_branding == "Chrome") || (is_win && current_cpu == "x64" && ffmpeg_branding == "Chrome") || (is_win && current_cpu == "x86" && ffmpeg_branding == "Chrome") || (use_linux_config && current_cpu == "x64" && ffmpeg_branding == "Chrome") || (use_linux_config && current_cpu == "x64" && ffmpeg_branding == "ChromeOS") || (use_linux_config && current_cpu == "x86" && ffmpeg_branding == "Chrome") || (use_linux_config && current_cpu == "x86" && ffmpeg_branding == "ChromeOS")) {',
    ffmpeg_c_sources: [
      'libavcodec/x86/bswapdsp_init.c',
      'libavcodec/x86/hevcdsp_init.c',
    ],
    ffmpeg_asm_sources: [
      'libavcodec/x86/bswapdsp.asm',
      'libavcodec/x86/hevc_add_res.asm',
      'libavcodec/x86/hevc_deblock.asm',
      'libavcodec/x86/hevc_idct.asm',
      'libavcodec/x86/hevc_mc.asm',
      'libavcodec/x86/hevc_sao.asm',
      'libavcodec/x86/hevc_sao_10bit.asm',
    ]
  },
  {
    condition: 'if ((is_apple && current_cpu == "arm64" && ffmpeg_branding == "Chrome") || (is_win && current_cpu == "arm64" && ffmpeg_branding == "Chrome") || (use_linux_config && current_cpu == "arm64" && ffmpeg_branding == "Chrome") || (use_linux_config && current_cpu == "arm64" && ffmpeg_branding == "ChromeOS")) {',
    ffmpeg_c_sources: [
      'libavcodec/aarch64/hevcdsp_init_aarch64.c',
    ],
    ffmpeg_gas_sources: [
      'libavcodec/aarch64/autorename_libavcodec_aarch64_hevcdsp_idct_neon.S',
      'libavcodec/aarch64/autorename_libavcodec_aarch64_hevcdsp_sao_neon.S',
      'libavcodec/aarch64/hevcdsp_deblock_neon.S',
      'libavcodec/aarch64/hevcdsp_qpel_neon.S',
    ]
  },
  {
    condition: 'if ((use_linux_config && current_cpu == "arm" && arm_use_neon && ffmpeg_branding == "Chrome") || (use_linux_config && current_cpu == "arm" && arm_use_neon && ffmpeg_branding == "ChromeOS") || (use_linux_config && current_cpu == "arm" && ffmpeg_branding == "Chrome") || (use_linux_config && current_cpu == "arm" && ffmpeg_branding == "ChromeOS")) {',
    ffmpeg_c_sources: [
      'libavcodec/arm/hevcdsp_init_arm.c',
    ]
  },
  {
    condition: 'if ((use_linux_config && current_cpu == "arm" && arm_use_neon && ffmpeg_branding == "Chrome") || (use_linux_config && current_cpu == "arm" && arm_use_neon && ffmpeg_branding == "ChromeOS")) {',
    ffmpeg_c_sources: [
      'libavcodec/arm/hevcdsp_init_neon.c',
    ],
    ffmpeg_gas_sources: [
      'libavcodec/arm/hevcdsp_deblock_neon.S',
      'libavcodec/arm/hevcdsp_idct_neon.S',
      'libavcodec/arm/hevcdsp_qpel_neon.S',
      'libavcodec/arm/hevcdsp_sao_neon.S',
    ]
  }
]

function genPath(subPath) {
  return path.resolve(ffmpegRoot, subPath);
}

function modifyAVCodec(filename, value) {
  const content = fs.readFileSync(filename, encodingConfig);
  if (content.includes(value)) {
    return;
  }
  fs.writeFileSync(filename, content.replace('NULL };', `${value},\n    NULL };`), encodingConfig);
}

function writeAutoRenameFile(filename, content) {
  fs.writeFileSync(filename, content, encodingConfig);
}

function enableHevcConfig(filename) {
  let content = fs.readFileSync(filename, encodingConfig);
  content = content
    .replace('define CONFIG_HEVC_DECODER 0', 'define CONFIG_HEVC_DECODER 1')
    .replace('define CONFIG_HEVC_PARSER 0', 'define CONFIG_HEVC_PARSER 1')
    .replace('define CONFIG_HEVCPARSE 0', 'define CONFIG_HEVCPARSE 1')
    .replace('define CONFIG_HEVC_SEI 0', 'define CONFIG_HEVC_SEI 1')
    .replace('define CONFIG_BSWAPDSP 0', 'define CONFIG_BSWAPDSP 1')
    .replace('define CONFIG_DOVI_RPU 0', 'define CONFIG_DOVI_RPU 1')
    .replace("--enable-decoder='aac,h264'", "--enable-decoder='aac,h264,hevc'")
    .replace("--enable-parser='aac,h264'", "--enable-decoder='aac,h264,hevc'");
  fs.writeFileSync(filename, content, encodingConfig);
}

function modifyHevcBuildFFmpegPy(filename) {
  let content = fs.readFileSync(filename, encodingConfig);
  content = content
    .replace('--enable-decoder=aac,h264', '--enable-decoder=aac,h264,hevc')
    .replace('--enable-parser=aac,h264', '--enable-parser=aac,h264,hevc');
  fs.writeFileSync(filename, content, encodingConfig);
}

function enableFFMPEGHevc(brand, os, arch) {
  modifyAVCodec(genPath(`./chromium/config/${brand}/${os}/${arch}/libavcodec/codec_list.c`), '&ff_hevc_decoder');
  modifyAVCodec(genPath(`./chromium/config/${brand}/${os}/${arch}/libavcodec/parser_list.c`), '&ff_hevc_parser');
  if (os !== 'win-msvc') {
    enableHevcConfig(genPath(`./chromium/config/${brand}/${os}/${arch}/config_components.h`));
  }
  enableHevcConfig(genPath(`chromium/config/${brand}/${os}/${arch}/config.h`));
  if ((arch === 'x64' || arch === 'ia32') && (os === 'win' || os == 'win-msvc' || os == 'mac')) {
    enableHevcConfig(genPath(`chromium/config/${brand}/${os}/${arch}/config.asm`));
  }
}

function modifyFFMPEGGenerated(filename) {
  let content = fs.readFileSync(filename, encodingConfig);
  for (const patch of patches) {
    if (!content.includes(patch.condition)) {
      console.error(`Failed to modify ffmpeg_generated.gni, please upgrade the script!`);
      process.exit(1);
    }
    let toAdd = '';
    for (const [field, sources] of Object.entries(patch)) {
      if (field === 'condition') {
        continue;
      }
      toAdd += `
  ${field} += [
${sources.map(source => `    "${source}"`).join(',\n')}
  ]
`;
    }
    if (!content.includes(toAdd)) {
      content = content
        .replace(patch.condition, patch.condition + '\n' + toAdd);
    }
  }
  fs.writeFileSync(filename, content, encodingConfig);
}

function enableSoftwreDecodeHEVC() {

  // 1. Add hevc decorder param for ffmpeg
  enableFFMPEGHevc('Chrome', 'win', 'ia32');
  enableFFMPEGHevc('Chrome', 'win', 'x64');
  enableFFMPEGHevc('Chrome', 'win', 'arm64');
  enableFFMPEGHevc('Chrome', 'win-msvc', 'ia32');
  enableFFMPEGHevc('Chrome', 'win-msvc', 'x64');
  enableFFMPEGHevc('Chrome', 'mac', 'x64');
  enableFFMPEGHevc('Chrome', 'mac', 'arm64');
  enableFFMPEGHevc('Chrome', 'ios', 'arm64');
  enableFFMPEGHevc('Chrome', 'linux', 'x64');
  enableFFMPEGHevc('Chrome', 'linux', 'ia32');
  enableFFMPEGHevc('Chrome', 'linux', 'arm64');
  enableFFMPEGHevc('Chrome', 'linux', 'arm');
  enableFFMPEGHevc('Chrome', 'linux', 'arm-neon');

  enableFFMPEGHevc('Chromium', 'win', 'ia32');
  enableFFMPEGHevc('Chromium', 'win', 'x64');
  enableFFMPEGHevc('Chromium', 'win', 'arm64');
  enableFFMPEGHevc('Chromium', 'win-msvc', 'ia32');
  enableFFMPEGHevc('Chromium', 'win-msvc', 'x64');
  enableFFMPEGHevc('Chromium', 'mac', 'x64');
  enableFFMPEGHevc('Chromium', 'mac', 'arm64');
  enableFFMPEGHevc('Chromium', 'ios', 'arm64');
  enableFFMPEGHevc('Chromium', 'linux', 'x64');
  enableFFMPEGHevc('Chromium', 'linux', 'ia32');
  enableFFMPEGHevc('Chromium', 'linux', 'arm64');
  enableFFMPEGHevc('Chromium', 'linux', 'arm');
  enableFFMPEGHevc('Chromium', 'linux', 'arm-neon');
  enableFFMPEGHevc('Chromium', 'linux-noasm', 'x64');

  enableFFMPEGHevc('ChromeOS', 'linux', 'x64');
  enableFFMPEGHevc('ChromeOS', 'linux', 'ia32');
  enableFFMPEGHevc('ChromeOS', 'linux', 'arm64');
  enableFFMPEGHevc('ChromeOS', 'linux', 'arm');
  enableFFMPEGHevc('ChromeOS', 'linux', 'arm-neon');
  enableFFMPEGHevc('ChromeOS', 'linux-noasm', 'x64');

  // 2. Modify build_ffmpeg.py
  modifyHevcBuildFFmpegPy(genPath(`./chromium/scripts/build_ffmpeg.py`));

  // 3. Create auto rename file
  const prefix = '// File automatically generated. See crbug.com/495833.';
  writeAutoRenameFile(genPath(`./libavcodec/aarch64/autorename_libavcodec_aarch64_hevcdsp_idct_neon.S`), [prefix, '#include "hevcdsp_idct_neon.S"'].join('\n'));
  writeAutoRenameFile(genPath(`./libavcodec/aarch64/autorename_libavcodec_aarch64_hevcdsp_sao_neon.S`), [prefix, '#include "hevcdsp_sao_neon.S"'].join('\n'));
  writeAutoRenameFile(genPath(`./libavcodec/autorename_libavcodec_bswapdsp.c`), [prefix, '#include "bswapdsp.c"'].join('\n'));

  // 4. Modify ffmpeg_generated.gni
  modifyFFMPEGGenerated(genPath(`./ffmpeg_generated.gni`));

  // 5. Commit the code then `git format-patch HEAD^ -o /path/to/export/patch`
  // if you want to generate the `.patch` file.
  if (genPatch) {
    // Commit the code.
    try {
      const msg = [
        "Video: Add HEVC ffmpeg decoder & parser",
        "Add ffmpeg software decoder and parser for HEVC to enable SW HEVC decoding"
      ].map(v => `-m "${v}" `).join('');
      childProcess.execSync(`git add -A && git commit ${msg}`, {
        cwd: ffmpegRoot
      });
    } catch(e) {}
    // Gen patch.
    try {
      childProcess.execSync(`git format-patch HEAD"^" -o "${output}"`, {
        cwd: ffmpegRoot
      });
      console.log('Generate patch success!');
    } catch (e) {}
  }
}

enableSoftwreDecodeHEVC();
console.log('Modify ffmpeg success!');