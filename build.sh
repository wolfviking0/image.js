make

echo "function loadImage(filename,data) {" > libimage.js
echo "\tvar Module = {" >> libimage.js
echo "\t\timagename: filename," >> libimage.js
echo "\t\timagedata: data" >> libimage.js
echo " \t};" >> libimage.js
cat libimage.raw.js >> libimage.js
echo "\t\tvar ret = {" >> libimage.js
echo "\t\t\"width\": Module['ccall']('width', 'number', null)," >> libimage.js
echo "\t\t\"height\": Module['ccall']('height', 'number', null)," >> libimage.js
echo "\t\t\"format\": Module['ccall']('format', 'number', null)," >> libimage.js
echo "\t\t\"raw\": Ha.e['image.raw'].e">> libimage.js
#echo "\t\t\"raw\": FS.root.contents['image.raw'].contents">> libimage.js
echo "\t\t};" >> libimage.js
echo "\t\treturn ret" >> libimage.js
echo "}" >> libimage.js

