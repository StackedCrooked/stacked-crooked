(import java.awt.Color)
(import java.awt.Dimension)
(import java.awt.Toolkit)
(import java.awt.event.KeyListener)
(import javax.swing.JFrame)
(import javax.swing.JPanel)

(use 'clojure.contrib.math)

(def prefs {
  :block-width 10
  :block-height 10
  :screen-width 320
  :screen-height 240
  :border-left 7
})

(def iBlock {
  :grids
  [
    [ [ 0 1 ]
      [ 0 1 ]
      [ 0 1 ]
      [ 0 1 ] ]    

    [ [ 0 0 0 0 ]
      [ 1 1 1 1 ] ]
  ]  
})

(def sBlock {
  :grids
  [
    [ [ 0 1 1 ]
      [ 1 1 0 ] ] 
   
    [ [ 0 1 0 ]
      [ 0 1 1 ]
      [ 0 0 1 ] ]
  ]  
})

(def zBlock {
  :grids
  [
    [ [ 1 1 0 ]
      [ 0 1 1 ] ]  
  
    [ [ 0 0 1 ]
      [ 0 1 1 ]
      [ 0 1 0 ] ]
  ]  
})

(def oBlock {
  :grids
  [
    [ [ 0 0 0 ]
      [ 0 1 1 ]
      [ 0 1 1 ] ]
  ]  
})

(def tBlock {
  :grids
  [
    [ [ 1 1 1 ]
      [ 0 1 0 ] ]
      
    [ [ 0 1 0 ]
      [ 1 1 0 ]
      [ 0 1 0 ] ]
      
    [ [ 0 1 0 ]
      [ 1 1 1 ] ]
      
    [ [ 0 1 0 ]
      [ 0 1 1 ]
      [ 0 1 0 ] ]
  ]  
})

(def lBlock {
  :grids
  [
    [ [ 1 0 0 ]
      [ 1 0 0 ]
      [ 1 1 0 ] ]

    [ [ 0 0 0 ]
      [ 1 1 1 ]
      [ 1 0 0 ] ]

    [ [ 1 1 0 ]
      [ 0 1 0 ]
      [ 0 1 0 ] ]

    [ [ 0 0 0 ]
      [ 0 0 1 ]
      [ 1 1 1 ] ]
  ]
})

(def jBlock {
  :grids
  [
    [ [ 0 1 0 ]
      [ 0 1 0 ]
      [ 1 1 0 ] ]

    [ [ 0 0 0 ]
      [ 1 0 0 ]
      [ 1 1 1 ] ]

    [ [ 1 1 0 ]
      [ 1 0 0 ]
      [ 1 0 0 ] ]

    [ [ 0 0 0 ]
      [ 1 1 1 ]
      [ 0 0 1 ] ]
  ]
})

(def blockTypes [iBlock sBlock zBlock oBlock tBlock lBlock jBlock])

(defn randomBlock []
  (let [idx (mod (round (rand 1000)) (count blockTypes))]
    (nth blockTypes idx)))

(def gamestate {
  :field {
    :rows [
    [ 0 0 0 0 0 0 0 0 0 0 ]
    [ 0 0 0 0 0 0 0 0 0 0 ]
    [ 0 0 0 0 0 0 0 0 0 0 ]
    [ 0 0 0 0 0 0 0 0 0 0 ]
    [ 0 0 0 0 0 0 0 0 0 0 ]
    
    [ 0 0 0 0 0 0 0 0 0 0 ]
    [ 0 0 0 0 0 0 0 0 0 0 ]
    [ 0 0 0 0 0 0 0 0 0 0 ]
    [ 0 0 0 0 0 0 0 0 0 0 ]
    [ 0 0 0 0 0 0 0 0 0 0 ]
    
    [ 0 0 0 0 0 0 0 0 0 0 ]
    [ 0 0 0 0 0 0 0 0 0 0 ]
    [ 0 0 0 0 0 0 0 0 0 0 ]
    [ 0 0 0 0 0 0 0 0 0 0 ]
    [ 0 0 0 0 0 0 0 0 0 0 ]
    
    [ 0 0 0 0 0 0 0 0 0 0 ]
    [ 0 0 0 0 0 0 0 0 0 0 ]
    [ 0 0 0 0 0 0 0 0 0 0 ]
    [ 0 0 0 0 0 0 0 0 0 0 ]
    [ 0 0 0 0 0 0 0 0 0 0 ]
    ]
  }
  :block {
    :type (ref lBlock)
    :rotation (ref 0)
    :x (ref 0)
    :y (ref 0)
  }})


(defn centerInScreen [frame]
  (let [  dim (.getScreenSize(Toolkit/getDefaultToolkit))
          w (.width (.getSize frame))
          h (.height (.getSize frame))
          x (int (/ (- (.width dim) w) 2))
          y (int (* 0.75(/ (- (.height dim) h) 2)))]
  (.setLocation frame x y)))

(defn drawRectangle [g x y w h c]
  (doto g
    (.setColor c)
    (.fillRect (* w x) (* h y) w h)))
    
(defn drawRow [g x y row]
  (let [w (prefs :block-width)
        h (prefs :block-height)]
  (dotimes [i (count row)]
    (if (== 1 (nth row i))
      (drawRectangle g (+ i x) y w h (java.awt.Color/BLUE))))))
      
(defn drawBlock [g block x y]
  (let [  block     (gamestate :block)
          blockType (deref (block :type))
          rotation  (deref (block :rotation))
          grids     (blockType :grids)
          gridIdx   (mod rotation (count grids))
          rows      (nth grids gridIdx) ]
  (dotimes [i (count rows)] (drawRow g (+ (prefs :border-left) (deref x)) (+ i (deref y)) (nth rows i)))))
      
(defn drawField [g field]
  (let [ rows (field :rows)]
    (dotimes [i (count rows)]
      (drawRow g 0 (+ i 10) (nth rows i)))))
    
(defn drawGameState [gs g]
  (let [ f (gs :field)
         b (gs :block)         
         x (b :x)
         y (b :y) ]
    (drawBlock g b x y)))
    
(defn rotate [block]
  (dosync (alter (block :rotation) inc)))
  

(defn nextBlock []
  (dosync (alter ((gamestate :block) :type) (fn [oldBlock] (randomBlock)))))
  
(defn firstNonZeroElement [row]
  (count (take-while (fn [x] (== x 0)) row)))
  

(defn firstIf [collection predicate]
  (count (take-while (fn [x] (not (predicate x))) collection)))

(defn minLeft [b]
  (let [blockType (deref (b :type))
        rotation  (deref (b :rotation))
        grids     (blockType :grids)
        gridIdx   (mod rotation (count grids))
        rows      (nth grids gridIdx) ]
    (* -1 (firstNonZeroElement
      (reduce (fn [x y] (if (< (firstNonZeroElement x) (firstNonZeroElement y))
                        x y)) rows)))))

(defn moveLeft [b x]
    (if (< (minLeft b) (deref x))
      (dosync (alter x dec))))

(defn createPanel [gs x y]
  (doto
    (proxy [JPanel KeyListener] []
      (paintComponent [g]
        (proxy-super paintComponent g)
        (drawGameState gs g))
      (getPreferredSize [] (Dimension. 320 240))
      (keyPressed [e]
        (let [keyCode (.getKeyCode e)]
          (if (== 37 keyCode) (moveLeft (gs :block) x)
          (if (== 38 keyCode) (rotate (gs :block))
          (if (== 39 keyCode) (dosync (alter x inc))
          (if (== 40 keyCode) (dosync (alter y inc))
          (if (== 32 keyCode) (nextBlock)
                              (println keyCode))))))))
      (keyReleased [e])
      (keyTyped [e]))
    (.setFocusable true)))

(def panel
  (createPanel gamestate
    ((gamestate :block) :x)
    ((gamestate :block) :y)))

(defn main []
  (let [frame (JFrame. "Test")]
    (doto frame
      (.add panel)
      (.pack)
      (.setDefaultCloseOperation JFrame/EXIT_ON_CLOSE)    
      (.setVisible true))
    (.addKeyListener panel panel)
    (centerInScreen frame)
    (loop []
      (.repaint panel)
      (Thread/sleep 10)
      (recur))))

;(main)
