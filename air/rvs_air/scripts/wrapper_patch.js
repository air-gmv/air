function removeArguments(argumentList, valuesToRemove) {
   for (var i = argumentList.length; i >= 0; i--) {
      if (valuesToRemove.indexOf(argumentList[i]) != -1) {
         argumentList.splice(i, 1);
      }
   }
}
 
 
modLinker = function(linker, argumentList, rvsFiles, outputFile) {
 
   print("moving RVS args...");
   removeArguments(argumentList, rvsFiles);
 
   var loc = 0;
   for (var i = 0; i < argumentList.length; i++) {
      if (argumentList[i].endsWith("-LAIR")) {
         loc = i;
         break;
      }
   }
 
   for (var j = 0; j < rvsFiles.length; j++) {
      argumentList.splice(loc, 0, rvsFiles[j]);
   }
}