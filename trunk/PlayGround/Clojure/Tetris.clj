

(in-ns 'tetris)
(clojure.core/use 'clojure.core)
(import java.awt.Color)
(import java.awt.Dimension)
(import java.awt.Toolkit)
(import java.awt.event.KeyListener)
(import javax.swing.JFrame)
(import javax.swing.JPanel)
(use 'clojure.contrib.math)

; Grid data structure
; -------------------
(defstruct grid :width :height)

(defn create-grid [w h initial-value]
  (struct-map grid
    :width  w
    :height h
    :data   (ref (vec (repeat (* w h) initial-value)))))

(defn create-grid-with-data [w h data]
  (struct-map grid
    :width w
    :height h
    :data (ref data)))
    
(defn get-grid [g x y]
  (let [data  (deref (g :data))
        idx   (+ x (* (g :width) y)) ]
    (nth data idx)))
    
(defn set-grid [g x y value]
  (let [data  (deref (g :data))
        idx   (+ x (* (g :width) y)) ]
    (dosync (alter (g :data) (fn [_] (assoc data idx value))))))

(defn get-grid-rows [g]
  (partition (g :width) (deref (g :data))))

; Shuffle utility function
; ------------------------    
(defn sort-by-mem-keys 
  "Like clojure/sort-by but only calls keyfn once on each item in coll." 
  ([keyfn coll] 
     (sort-by-mem-keys keyfn compare coll)) 
  ([keyfn #^java.util.Comparator comp coll] 
     (let [key-vals (map (fn [x] [(keyfn x) x]) coll) 
           sorted-pairs (sort (fn [x y] (.compare comp (first x) (first y))) 
                              key-vals)] 
       (map second sorted-pairs))))

(defn shuffle 
  "Returns a seq of coll shuffled in O(n log n) time.
   Space is at least O(2N). Each item in coll is assigned
   a random number which becomes the sort key." 
  [coll]
  (println "shuffle")
  (sort-by-mem-keys (fn [_] (rand)) coll))


(def prefs {
  :num-rows 20
  :num-columns 10
  :block-width 15
  :block-height 15
  :screen-width 500
  :screen-height 400
  :border-left 1
  :border-left-debug 12
  :border-top 2 })


(def i-block {
  :value 1
  :grids [  (create-grid-with-data 2 4 [ 0 1
                                         0 1
                                         0 1
                                         0 1])

            (create-grid-with-data 4 1 [ 1 1 1 1 ])]})

(def j-block {
  :value 2
  :grids [  (create-grid-with-data 3 3 [ 0 2 0
                                         0 2 0
                                         2 2 0 ])

            (create-grid-with-data 3 3 [ 0 0 0
                                         2 0 0
                                         2 2 2 ])

            (create-grid-with-data 3 3 [ 2 2 0
                                         2 0 0
                                         2 0 0 ])

            (create-grid-with-data 3 3 [ 0 0 0
                                         2 2 2
                                         0 0 2 ])]})

(def l-block {
  :value 3
  :grids [  (create-grid-with-data 3 3 [ 3 0 0
                                         3 0 0
                                         3 3 0 ])

            (create-grid-with-data 3 3 [ 0 0 0
                                         3 3 3
                                         3 0 0 ])

            (create-grid-with-data 3 3 [ 3 3 0
                                         0 3 0
                                         0 3 0 ])

            (create-grid-with-data 3 3 [ 0 0 0
                                         0 0 3
                                         3 3 3 ])]})


(def o-block {
  :value 4
  :grids [ (create-grid-with-data 2 2 [ 4 4 4 4 ])]})

(def s-block {
  :value 5
  :grids [ (create-grid-with-data 3 2 [  0 5 5 
                                         5 5 0 ])

           (create-grid-with-data 3 3 [  0 5 0 
                                         0 5 5
                                         0 0 5 ])]})

(def t-block {
  :value 6
  :grids [ (create-grid-with-data 3 2 [  6 6 6
                                         0 6 0 ])

           (create-grid-with-data 3 3 [  0 6 0 
                                         6 6 0
                                         0 6 0 ]) 

           (create-grid-with-data 3 2 [  0 6 0
                                         6 6 6 ]) 

           (create-grid-with-data 3 3 [  0 6 0
                                         0 6 6
                                         0 6 0 ])]})

(def z-block {
  :value 7
  :grids [ (create-grid-with-data 3 2 [ 7 7 0
                                        0 7 7 ])  

           (create-grid-with-data 3 3 [ 0 0 7
                                        0 7 7
                                        0 7 0 ])]})
  
(def active-block {
    :type (ref l-block)
    :rotation (ref 0)
    :x (ref 0)
    :y (ref 0) })

(def block-types [i-block j-block l-block o-block s-block t-block z-block])


(def bag-of-blocks
  (ref [i-block j-block l-block o-block s-block t-block z-block]))

(def bag-index (ref 0))

(def global-field (create-grid 10 20 0))

(defn random-block []
  (let [ bag       (deref bag-of-blocks)
         bag-size  (count bag) ]
    (if (== (deref bag-index) bag-size)
      (do     
        (dosync (alter bag-index (fn [n] 0)))
        (dosync (alter bag-of-blocks shuffle))))
    (let [ result (nth bag (deref bag-index)) ]
      (dosync (alter bag-index inc))
      (println "" (deref bag-index) "" (result :value))
      result)))


  
(defn get-color [grid-value]
  (let [color-table [ Color/BLACK
                      Color/CYAN
                      Color/BLUE
                      (Color. 255 165 0)
                      Color/YELLOW
                      Color/GREEN
                      (Color. 170 0 255)
                      Color/RED] ]
    (if (and (not (nil? grid-value))
             (>= grid-value 0)
             (< grid-value (count color-table)))
      (nth color-table grid-value)
      (Color/WHITE))))


(defn center-in-screen [frame]
  (let [  dim (.getScreenSize(Toolkit/getDefaultToolkit))
          w (.width (.getSize frame))
          h (.height (.getSize frame))
          x 0 ; move window to the top-left of the monitor for easier debugging
          y 0 ] ;(int (* 0.75(/ (- (.height dim) h) 2)))
  (.setLocation frame x y)))

(defn draw-rectangle [g x y w h color]
  (doto g
    (.setColor color)
    (.fillRect (* w x) (* h y) w h)))

(defn draw-text [g x y w h text]
  (doto g
    (.drawString text (* w x) (* h y))))

(defn draw-block [g block]
  (let [  x  	        (deref (block :x))
          y           (deref (block :y))
          rotation    (deref (block :rotation))
          grids       ((deref (block :type)) :grids)
          grid-idx    (mod rotation (count grids))
          active-grid (grids grid-idx)]
  (dotimes [i (active-grid :width)]
      (dotimes [j (active-grid :height)]
        (let [cell-value (get-grid active-grid i j)]
          (if-not (zero? cell-value)
            (let [paint-x   (+ (prefs :border-left) (+ x i))
                  paint-y   (+ (prefs :border-top) (+ y j))]
              (draw-rectangle g
                              paint-x
                              paint-y
                              (prefs :block-width)
                              (prefs :block-height)
                              (get-color cell-value)))))))))

(defn draw-field [g field]
  (dotimes [x (field :width)]
    (dotimes [y (field :height)]
      (draw-rectangle g (+ (prefs :border-left) x)
                        (+ (prefs :border-top) y)
                        (prefs :block-width)
                        (prefs :block-height)
                        (get-color (get-grid field x y))))))

(defn draw-all [g f b]
  (draw-field g f)
  (draw-block g b))

(defn check-position-valid [field block]
  (let [stop-condition (ref false)
        grids          ((deref (block :type)) :grids)
        grid-idx       (mod (deref (block :rotation)) (count grids))
        grid           (grids grid-idx)  ]
      (loop [ x 0 ]
        (when (and (not (deref stop-condition))
                   (< x (grid :width)))
          (loop [ y 0 ]
            (when (and (not (deref stop-condition))
                       (< y (grid :height)))
              (let [ block-value (get-grid grid x y)
                     field-x     (+ x (deref (block :x)))
                     field-y     (+ y (deref (block :y))) ]
                (if (not (zero? block-value))
                  (if-not
                    (and (>= field-x 0)
                         (< field-x (field :width))
                         (< field-y (field :height))
                         (zero? (get-grid field field-x field-y)))
                    (dosync (alter stop-condition (fn [_] true))))))
              (recur (inc y))))
          (recur (inc x))))
  (not (deref stop-condition))))


(defn rotate [block]
  (dosync (alter (block :rotation) inc)))

(defn next-block []
  (dosync
    (alter (active-block :type) (fn [_] (random-block)))
    (alter (active-block :rotation) (fn [_] 0))
    (alter (active-block :x)
      (fn [_]
        (let [  block-type  (deref (active-block :type))
                block-x     (deref (active-block :x))
                block-y     (deref (active-block :y))
                grids       (block-type :grids)
                grid-idx    (mod (deref (active-block :rotation)) (count grids))
                block-grid  (grids grid-idx)]
          (int (/ (- (global-field :width)
                     (block-grid :width)) 2)))))
    (alter (active-block :y) (fn [_] 0))))

(defn move-left [f b]
  (dosync (alter (b :x) dec))
  (if-not (check-position-valid f b)
    (dosync (alter (b :x) inc))))
    
(defn move-right [f b]
  (dosync (alter (b :x) inc))
  (if-not (check-position-valid f b)
    (dosync (alter (b :x) dec))))

(defn commit-block [field block]
  (let [  block-type  (deref (block :type))
          block-x     (deref (block :x))
          block-y     (deref (block :y))
          grids       (block-type :grids)
          grid-idx    (mod (deref (block :rotation)) (count grids))
          block-grid  (grids grid-idx)]
    (dotimes [x (block-grid :width)]
      (dotimes [y (block-grid :height)]
        (let [value (get-grid block-grid x y)]
            (if-not (zero? value)
              (set-grid field (+ x block-x) (+ y block-y) value)))))))

(defn move-down [f b]
  (dosync (alter (b :y) inc))
  (if (not (check-position-valid f b))
    (do
      (dosync (alter (b :y) dec))
      (commit-block f b)
      (next-block))))

(defn create-panel []
  (doto
    (proxy [JPanel KeyListener] []
      (paintComponent [g]
        (proxy-super paintComponent g)
        (draw-all g global-field active-block))
      (getPreferredSize [] (Dimension. (prefs :screen-width)
                                       (prefs :screen-height)))
      (keyPressed [e]
        (let [keyCode (.getKeyCode e)]          
          (.repaint this)
          (if (== 37 keyCode) (move-left global-field active-block)
          (if (== 38 keyCode) (rotate active-block)
          (if (== 39 keyCode) (move-right global-field active-block)
          (if (== 40 keyCode) (move-down global-field active-block)
          (if (== 32 keyCode) (next-block)
                              (println keyCode))))))))
      (keyReleased [e])
      (keyTyped [e]))
    (.setFocusable true)))

(def panel
  (create-panel))

(defn main []
  (let [frame (JFrame. "Test")]
    (doto frame
      (.add panel)
      (.pack)
      (.setDefaultCloseOperation JFrame/EXIT_ON_CLOSE)    
      (.setVisible true))
    (.addKeyListener panel panel)
    (center-in-screen frame)
    (dosync (alter bag-of-blocks shuffle))
    (next-block)
    (loop []
      (Thread/sleep 10)
      (recur))))

(main)