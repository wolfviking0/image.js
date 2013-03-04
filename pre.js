
	Module['preRun'] = function() {
		FS.createDataFile('/', imagename,imagedata, true, true);
  	};
  	Module.arguments = [imagename];
 	Module['return'] = function() {
 		// Like that i can found the equivalent with -O2 and closure
		return FS.root.contents['image.raw'].contents;
 	};
 	Module['print'] = function(text) {
 		var element = document.getElementById('output');
        element.value += text + "\n";
        element.scrollTop = 99999; // focus on bottom
 	};


