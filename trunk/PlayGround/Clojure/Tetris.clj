(in-ns 'tetris)
(clojure.core/use 'clojure.core)
(import java.awt.Color)
(import java.awt.Dimension)
(import java.awt.Toolkit)
(import java.util.Timer)
(import java.util.TimerTask)
(import java.awt.event.KeyListener)
(import javax.swing.JFrame)
(import javax.swing.JPanel)
(use 'clojure.contrib.math)

(def prefs {
  :grid-x 10
  :grid-y 10
  :num-rows 20
  :num-columns 10
  :block-width 15
  :block-height 15
  :screen-width 500
  :screen-height 400
  :border-left 1
  :border-left-debug 12
  :border-top 2 })


; Grid data structure
; -------------------
(defstruct grid :width :height)

(defn create-grid [w h initial-value]
  (struct-map grid
    :width  w
    :height h
    :data   (ref (vec (repeat (* w h) initial-value)))))

(defn reset-grid [g initial-value]
  (alter (g :data)
           (fn [_] (vec (repeat (* (g :width) (g :height))
                                   initial-value)))))

(defn create-grid-with-data [w h data]
  (struct-map grid
    :width w
    :height h
    :data (ref data)))
    
(defn get-grid [g x y]
  (let [data  @(g :data)
        idx   (+ x (* (g :width) y)) ]
    (nth data idx)))
    
(defn set-grid [g x y value]
  (let [data  @(g :data)
        idx   (+ x (* (g :width) y)) ]
    (alter (g :data) (fn [_] (assoc data idx value)))))

(defn get-grid-rows [g]
  (partition (g :width) @(g :data)))


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
  :grids [ (create-grid-with-data 2 2 [ 4 4
                                        4 4 ])]})

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

(def global-field (create-grid 10 20 0))

(def block-types
  [i-block j-block l-block o-block s-block t-block z-block])

(def bag-of-blocks
  (ref [i-block j-block l-block o-block s-block t-block z-block]))

(def bag-index (ref 0))

(def next-blocks (ref []))
(def num-next-blocks 1)

(defn init-blocks []
  (dosync    
    (reset-grid global-field 0)
    (alter bag-of-blocks ( fn [_] (shuffle block-types)))
    (alter (active-block :type) (fn [_] (first @bag-of-blocks)))
    (alter (active-block :x)
          (fn [x]
            (let [  block-type  @(active-block :type)
                    block-x     x
                    block-y     @(active-block :y)
                    grids       (block-type :grids)
                    grid-idx    (mod @(active-block :rotation) (count grids))
                    block-grid  (grids grid-idx)]
              (int (/ (- (global-field :width)
                         (block-grid :width)) 2)))))
    (if-not (zero? num-next-blocks)
      (do
        (println "@bag-of-blocks" (count @bag-of-blocks))
        (alter next-blocks
          (fn [_] (subvec (vec @bag-of-blocks) 1 (inc num-next-blocks))))))
    (alter bag-of-blocks
           (fn [_] (drop (inc num-next-blocks) @bag-of-blocks)))))

(declare check-position-valid)
(declare game-over)

(defn next-block []
  (alter  (active-block :type)
          (fn [_]
            (if-not (zero? num-next-blocks)
              (first @next-blocks)
              (first @bag-of-blocks))))
  (if-not (zero? num-next-blocks)
    (alter next-blocks (fn [nb] (conj (vec (drop 1 nb)) (first @bag-of-blocks)))))
  (alter bag-of-blocks (fn [v] (vec (rest v))))
  (if (empty? @bag-of-blocks)
    (alter bag-of-blocks (fn [_] (shuffle block-types))))
  (alter (active-block :rotation) (fn [_] 0))
  (alter (active-block :x)
    (fn [x]
      (let [  block-type  @(active-block :type)
              block-x     x
              block-y     @(active-block :y)
              grids       (block-type :grids)
              grid-idx    (mod @(active-block :rotation) (count grids))
              block-grid  (grids grid-idx)]
        (int (/ (- (global-field :width)
                   (block-grid :width)) 2)))))
  (alter (active-block :y) (fn [_] 0))
  (if-not (check-position-valid global-field active-block)
    (game-over)))
  



(def lines (ref 0))

; speed of falling
; number of milliseconds between falling one row
; These are the same values as for the Gameboy:
;   http://tetris.wikia.com/wiki/Tetris_(Game_Boy)
(def level-speed-mapping [ 887 820 753 686 619
                           552 469 368 285 184
                           167 151 134 117 100
                           100 84  84  67  67  50 ])

(defn get-game-speed [level]
  (if (< level (count level-speed-mapping))
    (nth level-speed-mapping level)
    (last level-speed-mapping)))

; the number of lines that have to be scored in order to increment level
(def level-up-treshold 10)

; get-level returns level based on number of lines that have been scored.
; maximum level is 20, which means an interval of 20 ms between drops 
(defn get-level [lines]
  (let [level   (int (/ lines level-up-treshold))]
    level))

(def timer (ref (new Timer)))
(declare move-down)

(defn set-game-speed [interval]
  (let [task (proxy [TimerTask] []
               (run []
                (move-down global-field active-block)))]
    (.cancel @timer)
    (alter timer (fn [_] (new Timer)))
    (.scheduleAtFixedRate @timer task (long interval) (long interval))
    (println "Set gamespeed to" interval)
    (println "Lines" @lines)
    (println "Level" (get-level @lines))))

(defn random-block []
  (dosync
    (let [ bag       @bag-of-blocks
           bag-size  (count bag)    ]
    (if (== @bag-index bag-size)
      (do
        (alter bag-index (fn [n] 0))
        (alter bag-of-blocks shuffle)))
    (do
      (alter bag-index inc)
      (nth bag @bag-index)))))
  
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
          x 0
          y 0 ]
  (.setLocation frame x y)))

(defn paint-grid-cell [graphics i j value]
  (let [w   (prefs :block-width)
        h   (prefs :block-height)          
        x   (+ (prefs :grid-x) (* i w))
        y   (+ (prefs :grid-y) (* j h)) ]
    (doto graphics
      (.setColor (get-color value))
      (.fillRect x y w h))))

(defn draw-text [g x y w h text]
  (doto g
    (.drawString text (* w x) (* h y))))

(defn draw-block [g block]
  (let [  bx 	        @(block :x)
          by          @(block :y)
          rotation    @(block :rotation)
          grids       (@(block :type) :grids)
          grid-idx    (mod rotation (count grids))
          active-grid (grids grid-idx)]
  (dotimes [i (active-grid :width)]
      (dotimes [j (active-grid :height)]
        (let [cell-value (get-grid active-grid i j)]
          (if-not (zero? cell-value)
            (let [x  (+ (prefs :border-left) (+ bx i))
                  y  (+ (prefs :border-top) (+ by j))]
              (paint-grid-cell g x y cell-value))))))))

(defn clear-screen [panel g]
  (doto g
    (.setColor (Color/GRAY))
    (.fillRect 0 0 (.getWidth panel) (.getHeight panel))))
                              
(defn draw-field [g field]
  (dotimes [x (field :width)]
    (dotimes [y (field :height)]
      (paint-grid-cell g (+ (prefs :border-left) x)
                         (+ (prefs :border-top) y)
                         (get-grid field x y)))))

(defn draw-debug-field [g field]
  (dotimes [x (field :width)]
    (dotimes [y (field :height)]
      (draw-text g (+ 12 x)
                 (+ (prefs :border-top) y)
                 (prefs :block-width)
                 (prefs :block-height)
                 (str (get-grid field x y))))))

(defn draw-next-blocks [g blocks]
  (dotimes [ n (count blocks) ]
    (let [ block      (nth blocks n)
           grids      (block :grids)
           grid-idx   0
           grid       (nth grids grid-idx) ]
      (dotimes [ i (grid :width) ]
        (dotimes [ j (grid :height) ]
          (let [ x      (+ i (+ (prefs :border-left) (prefs :num-columns) 1))
                 y      (+ j (* n 5) (prefs :border-top))
                 value  (get-grid grid i j) ]
            (if-not (zero? value)
              (paint-grid-cell g x y value))))))))

(defn debug-draw-bag-of-blocks [g blocks]
  (dotimes [ n (count blocks) ]
    (let [ block      (nth blocks n)
           grids      (block :grids)
           grid-idx   0
           grid       (nth grids grid-idx) ]
      (dotimes [ i (grid :width) ]
        (dotimes [ j (grid :height) ]
          (let [ x      (+ i (+ (prefs :border-left) (prefs :num-columns) 5))
                 y      (+ j (* n 5) (prefs :border-top))
                 value  (get-grid grid i j) ]
            (if-not (zero? value)
              (paint-grid-cell g x y 0))))))))

(def frame-count
  (ref 0))

(defn current-time-ms []
  (.getTime (java.util.Date.)))

(def start-time
  (ref (current-time-ms)))

(def fps
  (ref 0))
  
(defn calculate-fps []
  (let [current-time        (current-time-ms)
        time-interval-ms    (- (current-time-ms) @start-time)
        time-interval-s     (/ time-interval-ms 1000) ]
    (dosync
      (alter fps (fn [_] (int (round (/ @frame-count time-interval-s)))))
      (alter frame-count (fn [_] 0))
      (alter start-time ( fn [_] (current-time-ms))))))

(defn draw-fps [g]
  (doto g
    (.setColor (Color/ORANGE))
    (.drawString (str "fps " @fps) 20 20)))
    
    
(defn draw-stats [g]
    (.setColor g (Color/LIGHT_GRAY))
    (.setFont g (.deriveFont (.getFont g) (int java.awt.Font/BOLD) (float 14)))
    (.drawString g (str "Lines " @lines) 300 60)
    (.drawString g (str "Level " (get-level @lines)) 300 80))

(defn draw-all [panel g f b]
  (clear-screen panel g)
  (if (zero? (mod @frame-count 100))
    (calculate-fps))
  (draw-fps g)
  (draw-field g f)
  (draw-next-blocks g @next-blocks)
  (draw-block g b)
  (draw-stats g))

(defn check-position-valid [field block]
  (let [grids          (@(block :type) :grids)
        grid-idx       (mod @(block :rotation) (count grids))
        grid           (grids grid-idx)  ]
      (every? true? (for [x (range 0 (grid :width))
                          y (range 0 (grid :height))
                          :let [block-value (get-grid grid x y)
                                field-x     (+ x @(block :x))
                                field-y     (+ y @(block :y))]]
                      (if (zero? block-value)
                        true 
                        (and (>= field-x 0)
                             (< field-x (field :width))
                             (< field-y (field :height))
                             (zero? (get-grid field field-x field-y))))))))

(defn rotate [field block]
  (dosync
    (alter (block :rotation) inc)
    (if-not (check-position-valid field block)
      (alter (block :rotation) dec))))

(defn move-left [f b]
  (dosync
    (alter (b :x) dec)
    (if-not (check-position-valid f b)
    (alter (b :x) inc))))

(defn move-right [f b]
  (dosync
    (alter (b :x) inc)
    (if-not (check-position-valid f b)
      (alter (b :x) dec))))

(defn contains [collection value]
  (> (count (filter (fn [el] (== value el)) collection)) 0))

(defn flatten [x]
  (let [s? #(instance? clojure.lang.Sequential %)]
    (filter (complement s?) (tree-seq s? seq x))))

(defn clear-lines [field]
  (let [  rows              (get-grid-rows field)
          remaining-rows    (filter (fn [row] (contains row 0)) rows)
          num-lines-scored  (- (count rows) (count remaining-rows))
          ]
    (if-not (zero? num-lines-scored)
      (do
        (alter (field :data)
          (fn [_]
            (let [zeroes (vec (repeat (* (field :width) num-lines-scored) 0))]
              (vec (flatten (conj zeroes remaining-rows))))))
        (alter lines (fn [lines] (+ lines num-lines-scored))))
      (set-game-speed (get-game-speed (get-level @lines))))))

(defn commit-block [field block]
  (let [  block-type  @(block :type)
          block-x     @(block :x)
          block-y     @(block :y)
          grids       (block-type :grids)
          grid-idx    (mod @(block :rotation) (count grids))
          block-grid  (grids grid-idx)]
    (dotimes [x (block-grid :width)]
      (dotimes [y (block-grid :height)]
        (let [value (get-grid block-grid x y)]
            (if-not (zero? value)
              (set-grid field (+ x block-x) (+ y block-y) value)))))))

(defn move-down [f b]
  (dosync
    (alter (b :y) inc)
    (if-not (check-position-valid f b)
      (do
        (alter (b :y) dec)
        (commit-block f b)
        (clear-lines f)
        (next-block)))))

(defn drop-block [f b]
  (dosync
    (loop []
      (when (check-position-valid f b)
        (alter (b :y) inc)
        (recur)))
    (alter (b :y) dec)
    (commit-block f b)
    (clear-lines f)
    (next-block)))

(defn create-panel []
  (doto
    (proxy [JPanel KeyListener] []
      (paintComponent [g]
        (proxy-super paintComponent g)
        (draw-all this g global-field active-block)
        (dosync (alter frame-count inc)))
      (getPreferredSize [] (Dimension. (prefs :screen-width)
                                       (prefs :screen-height)))
      (keyPressed [e]
        (let [keyCode (.getKeyCode e)]    
          (if (== 37 keyCode) (move-left global-field active-block)
          (if (== 38 keyCode) (rotate global-field active-block)
          (if (== 39 keyCode) (move-right global-field active-block)
          (if (== 40 keyCode) (move-down global-field active-block)
          (if (== 32 keyCode) (drop-block global-field active-block)
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
    (init-blocks)    
    (dosync
      (set-game-speed (get-game-speed (get-level @lines))))
    (loop []
      (.repaint panel)
      (Thread/sleep 7)
      (recur))))
      
; For now just start a new game
(defn game-over []
  (init-blocks))

; TODO
; ----
; - statistics
; - game over message
; - high-score (internet?)
; - scoring http://tetris.wikia.com/wiki/Scoring
; v drop block space bar
(main)
