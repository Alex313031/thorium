// Dark mode button.

document.querySelector('#darkbutton').onclick = function() {
        useDark = !useDark;
        toggleDarkMode(useDark);
        localStorage.setItem('dark-mode', useDark);
    }
