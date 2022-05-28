// Dark mode and local storage preference

function toggleDarkMode(useDark) {
        if (useDark) {
            document.documentElement.classList.add('dark');
            document.documentElement.classList.remove('light');
        }
        else {
            document.documentElement.classList.remove('dark');
            document.documentElement.classList.add('light');
        }
    }
    let useDark = window.matchMedia('(prefers-color-scheme: dark)').matches;
    if (!useDark && localStorage.getItem('dark-mode') === 'true') {
        useDark = true;
        toggleDarkMode(useDark);
    }
    else if (localStorage.getItem('dark-mode') === 'false') {
        useDark = false;
        toggleDarkMode(useDark);
    }
