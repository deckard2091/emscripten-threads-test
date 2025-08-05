// Set up Module before Emscripten script runs
var Module = {
  canvas: (function() {
    // Ensure canvas exists when this is called
    var canvas = document.getElementById('canvas');
    if (!canvas) {
      console.error('Canvas not found!');
      return null;
    }
    canvas.addEventListener('contextmenu', function(e) {
      e.preventDefault();
    });
    return canvas;
  })(),
  print: function(text) {
    console.log(text);
  },
  printErr: function(text) {
    console.error(text);
  },
  setStatus: function(text) {
    if (text) {
      console.log('Status:', text);
    }
  }
};