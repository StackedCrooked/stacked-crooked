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

(def i-block {
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

(def s-block {
  :grids
  [
    [ [ 0 1 1 ]
      [ 1 1 0 ] ] 
   
    [ [ 0 1 0 ]
      [ 0 1 1 ]
      [ 0 0 1 ] ]
  ]  
})

(def z-block {
  :grids
  [
    [ [ 1 1 0 ]
      [ 0 1 1 ] ]  
  
    [ [ 0 0 1 ]
      [ 0 1 1 ]
      [ 0 1 0 ] ]
  ]  
})

(def o-block {
  :grids
  [
    [ [ 0 0 0 ]
      [ 0 1 1 ]
      [ 0 1 1 ] ]
  ]  
})

(def t-block {
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

(def l-block {
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

(def j-block {
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

(def blockTypes [i-block s-block z-block o-block t-block l-block j-block])

(defn random-block []
  (let [idx (mod (round (rand 1000)) (count blockTypes))]
    (nth blockTypes idx)))

(def gamestate {
  :field {
    :num-columns 10
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
    :type (ref l-block)
    :rotation (ref 0)
    :x (ref 0)
    :y (ref 0)
  }})

  
(def color-map {
  0 (java.awt.Color/BLACK)
  1 (java.awt.Color/BLUE) })


(defn center-in-screen [frame]
  (let [  dim (.getScreenSize(Toolkit/getDefaultToolkit))
          w (.width (.getSize frame))
          h (.height (.getSize frame))
          x (int (/ (- (.width dim) w) 2))
          y (int (* 0.75(/ (- (.height dim) h) 2)))]
  (.setLocation frame x y)))

(defn draw-rectangle [g x y w h color]
  (doto g
    (.setColor color)
    (.fillRect (* w x) (* h y) w h)))
    
(defn draw-row [g x y row]
  (let [w (prefs :block-width)
        h (prefs :block-height)]
  (dotimes [i (count row)]
    (draw-rectangle g (+ i x) y w h (color-map (nth row i))))))
      
(defn draw-block [g block x y]
  (let [  block       (gamestate :block)
          block-type  (deref (block :type))
          rotation    (deref (block :rotation))
          grids       (block-type :grids)
          grid-idx    (mod rotation (count grids))
          rows        (nth grids grid-idx) ]
  (dotimes [i (count rows)] (draw-row  g
                                      (+ (prefs :border-left) (deref x))
                                      (+ i (deref y))
                                      (nth rows i)))))
      
(defn draw-field [g field]
  (let [ rows (field :rows)]
    (dotimes [i (count rows)]
      (draw-row  g
                (prefs :border-left)
                i
                (nth rows i)))))
    
(defn draw-gamestate [gs g]
  (let [ f (gs :field)
         b (gs :block)         
         x (b :x)
         y (b :y) ]
    (draw-field g (gamestate :field))
    (draw-block g b x y)))
    
(defn rotate [block]
  (dosync (alter (block :rotation) inc)))
  

(defn next-block []
  (dosync (alter ((gamestate :block) :type) (fn [oldBlock] (random-block)))))
  
(defn first-non-zero-element [row]
  (count (take-while (fn [x] (== x 0)) row)))
  

(defn first-if [collection predicate]
  (count (take-while (fn [x] (not (predicate x))) collection)))
  
(defn last-if [collection predicate]
  (- (dec (count collection)) (first-if (rseq collection) predicate)))
  
(defn max-x [b]
  (let [block-type  (deref (b :type))
        rotation    (deref (b :rotation))
        grids       (block-type :grids)
        grid-idx    (mod rotation (count grids))
        rows        (nth grids grid-idx) ]
    (last-if
      (reduce 
        (fn [r1 r2]
          (let [lastNonZeroValue (fn [row] (last-if row (comp not zero?)))]
            (if (> (lastNonZeroValue r1) (lastNonZeroValue r2)) r1 r2)))
        rows)
      (comp not zero?))))

(defn min-x [b]
  (let [block-type  (deref (b :type))
        rotation    (deref (b :rotation))
        grids       (block-type :grids)
        grid-idx    (mod rotation (count grids))
        rows        (nth grids grid-idx) ]
    (* -1 (first-non-zero-element
      (reduce (fn [x y] (if (< (first-non-zero-element x) (first-non-zero-element y))
                        x y)) rows)))))

(defn moveLeft [b x]
    (if (< (min-x b) (deref x))
      (dosync (alter x dec))))

(defn moveRight [b x]
  (if (< (+ (deref x) (max-x b)) (dec ((gamestate :field) :num-columns)))
    (dosync (alter x inc))))

(defn createPanel [gs x y]
  (doto
    (proxy [JPanel KeyListener] []
      (paintComponent [g]
        (proxy-super paintComponent g)
        (draw-gamestate gs g))
      (getPreferredSize [] (Dimension. 320 240))
      (keyPressed [e]
        (let [keyCode (.getKeyCode e)]
          (if (== 37 keyCode) (moveLeft (gs :block) x)
          (if (== 38 keyCode) (rotate (gs :block))
          (if (== 39 keyCode) (moveRight (gs :block) x)
          (if (== 40 keyCode) (dosync (alter y inc))
          (if (== 32 keyCode) (next-block)
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
    (center-in-screen frame)
    (loop []
      (.repaint panel)
      (Thread/sleep 10)
      (recur))))

;(main)
