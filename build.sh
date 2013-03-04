make

echo "// Images.js, (C) 2013 Anthony LIOT, https://github.com/wolfviking0/image.js\n" > libimage.js
echo "var ImageJS = {\n" >> libimage.js
echo "\tDecode: function(imagedata,imagename) {\n" >> libimage.js
echo "\t\tvar Module = {" >> libimage.js
#echo "\t\t\timagename: filename," >> libimage.js
#echo "\t\t\timagedata: content" >> libimage.js
echo "\t\t};" >> libimage.js
cat libimage.raw.js >> libimage.js
echo "\t\t\tvar ret = {" >> libimage.js
echo "\t\t\t\"width\": Module['ccall']('width', 'number', null)," >> libimage.js
echo "\t\t\t\"height\": Module['ccall']('height', 'number', null)," >> libimage.js
echo "\t\t\t\"format\": Module['ccall']('format', 'number', null)," >> libimage.js
#echo "\t\t\"raw\": Qb.b['image.raw'].b">> libimage.js
echo "\t\t\t\"raw\": FS.root.contents['image.raw'].contents">> libimage.js
echo "\t\t\t};" >> libimage.js
echo "\t\t\treturn ret;" >> libimage.js
echo "\t},\n" >> libimage.js
echo "};\n" >> libimage.js
