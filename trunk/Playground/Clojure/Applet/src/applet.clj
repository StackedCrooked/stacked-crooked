(ns applet
  (:import (java.awt Graphics Color Font RenderingHints)
           (javax.swing JApplet JButton))
  (:gen-class
     :extends javax.swing.JApplet))
 
(defn -paint [#^JApplet applet #^Graphics g]  ;type hint avoids Reflection, then it works without signing
  (let [width (.getWidth applet)
        height (.getHeight applet)]
    (doto g
      (.setColor (. Color YELLOW))
      (.fillRect 0 0 width height)
      (.setColor (. Color BLACK))
      (.drawRect 0 0 (dec width) (dec height))
      (.setFont (Font. "Serif" (. Font PLAIN) 24))
      (.drawString "Hello World!" 20 40))))
      
      
; C:\WINDOWS\system32\java.exe -cp "C:\clojure-1.1.0\*" clojure.main -i "D:\Development\Repositories\stacked-crooked\PlayGround\Clojure\Tetris.clj" -r