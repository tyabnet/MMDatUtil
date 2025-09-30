// lightweight client-side navigation helper (anchor links)
document.addEventListener('DOMContentLoaded', function(){
  // highlight active link
  var path = window.location.pathname.split('/').pop() || 'index.html';
  var links = document.querySelectorAll('.nav a');
  links.forEach(function(a){
    if (a.getAttribute('href') === path) a.classList.add('active');
  });
});
