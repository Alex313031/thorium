// Spun? localstorage.

$( '#spinner' ).click( function() {
	localStorage.setItem('spun', 'true');
	localStorage.getItem('spun');
	}
);
