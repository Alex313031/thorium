// Dark mode button.

document.querySelector('button').onclick = function() {
        useDark = !useDark;
        toggleDarkMode(useDark);
        localStorage.setItem('dark-mode', useDark);
    }
