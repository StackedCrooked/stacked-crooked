mkdir tmp
copy src\applet.clj tmp
cd tmp
java -Dclojure.compile.path=. -cp "C:\clojure-1.1.0\*";. clojure.lang.Compile applet
jar cf ../cljapp.jar .
cd ..
move cljapp.jar html
