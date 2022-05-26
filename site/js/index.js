const btn = document.getElementById('btn1');

btn.addEventListener('click', function onClick(event) {
  // change background color
  document.body.style.backgroundColor = '#24292f';

  // optionally change text color
  document.body.style.color = '#ffffff';
});
