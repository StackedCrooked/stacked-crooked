(import java.awt.Color)
(import java.awt.Dimension)
(import java.awt.Toolkit)
(import java.awt.event.KeyListener)
(import javax.swing.JFrame)
(import javax.swing.JPanel)

(use 'clojure.contrib.math)



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
  "Returns a seq of coll shuffled in O(n log n) time. Space is at least 
O(2N). Each item in coll is 
  assigned a random number which becomes the sort key." 
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
  :border-top 2
})

(def field
  (atom
    (vec
      (doall
        (for [_ (range (prefs :num-rows))]
          (vec (repeat (prefs :num-columns) 0)))))))

(defn get-field [rowIdx colIdx]
  (nth (nth @field rowIdx) colIdx))

(defn set-field [rowIdx colIdx value]
  (swap! field #(update-in % [rowIdx colIdx] (constantly value))))

(def i-block {
  :value 1
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

(def j-block {
  :value 2
  :grids
  [
    [ [ 0 2 0 ]
      [ 0 2 0 ]
      [ 2 2 0 ] ]

    [ [ 0 0 0 ]
      [ 2 0 0 ]
      [ 2 2 2 ] ]

    [ [ 2 2 0 ]
      [ 2 0 0 ]
      [ 2 0 0 ] ]

    [ [ 0 0 0 ]
      [ 2 2 2 ]
      [ 0 0 2 ] ]
  ]
})

(def l-block {
  :value 3
  :grids
  [
    [ [ 3 0 0 ]
      [ 3 0 0 ]
      [ 3 3 0 ] ]

    [ [ 0 0 0 ]
      [ 3 3 3 ]
      [ 3 0 0 ] ]

    [ [ 3 3 0 ]
      [ 0 3 0 ]
      [ 0 3 0 ] ]

    [ [ 0 0 0 ]
      [ 0 0 3 ]
      [ 3 3 3 ] ]
  ]
})

(def o-block {
  :value 4
  :grids
  [
    [ [ 0 0 0 ]
      [ 0 4 4 ]
      [ 0 4 4 ] ]
  ]  
})

(def s-block {
  :value 5
  :grids
  [
    [ [ 0 5 5 ]
      [ 5 5 0 ] ] 
   
    [ [ 0 5 0 ]
      [ 0 5 5 ]
      [ 0 0 5 ] ]
  ]  
})

(def t-block {
  :value 6
  :grids
  [
    [ [ 6 6 6 ]
      [ 0 6 0 ] ]
      
    [ [ 0 6 0 ]
      [ 6 6 0 ]
      [ 0 6 0 ] ]
      
    [ [ 0 6 0 ]
      [ 6 6 6 ] ]
      
    [ [ 0 6 0 ]
      [ 0 6 6 ]
      [ 0 6 0 ] ]
  ]  
})

(def z-block {
  :value 7
  :grids
  [
    [ [ 7 7 0 ]
      [ 0 7 7 ] ]  
  
    [ [ 0 0 7 ]
      [ 0 7 7 ]
      [ 0 7 0 ] ]
  ]  
})

(def active-block {
    :type (ref l-block)
    :rotation (ref 0)
    :rowIdx (ref 0)
    :colIdx (ref 0) })

(def block-types [i-block j-block l-block o-block s-block t-block z-block])


(def bag-of-blocks
  (ref [i-block j-block l-block o-block s-block t-block z-block]))

(def bag-index (ref 0))


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
  (let [  block-type  (deref (block :type))
          row      	  (deref (block :rowIdx))
          col         (deref (block :colIdx))
          rotation    (deref (block :rotation))
          grids       (block-type :grids)
          grid-idx    (mod rotation (count grids))
          rows        (nth grids grid-idx)
          num-rows    (count rows)]
  (dotimes [ri num-rows]
    (let [current-row   (nth rows ri)
          num-columns   (count current-row) ]
      (dotimes [ci num-columns]
        (let [cell-value (nth current-row ci)]
          (if-not (zero? cell-value)
            (let [x	(+ (prefs :border-left) (+ col ci))
                  y	(+ (prefs :border-top) (+ row ri))]                  
            (draw-rectangle g x y
                            (prefs :block-width)
                            (prefs :block-height)
                            (get-color cell-value))))))))))

(defn draw-field [g field]
  (let [ numRows (count @field) ]
    (dotimes [ rowIdx numRows ]
      (let [currentRow   (nth @field rowIdx)
            numCols      (count currentRow) ]
        (dotimes [ colIdx numCols ]
          (draw-rectangle g (+ (prefs :border-left) colIdx)
                            (+ (prefs :border-top) rowIdx)
                            (prefs :block-width)
                            (prefs :block-height)
                            (get-color (currentRow colIdx))))))))

(defn draw-debug [g field]
  (let [ numRows (count @field) ]
    (dotimes [ rowIdx numRows ]
      (let [currentRow   (nth @field rowIdx)
            numCols      (count currentRow) ]    
        (dotimes [ colIdx numCols ]
          (draw-text g (+ (prefs :border-left-debug) colIdx)
                       (+ (prefs :border-top) rowIdx)
                       (prefs :block-width)
                       (prefs :block-height)
                       (str (get-field rowIdx colIdx))))))))

(defn draw-all [g]
  (let [ b active-block ]
    (draw-field g field)
    (draw-debug g field)
    (draw-block g b)))
    
(defn rotate [block]
  (dosync (alter (block :rotation) inc)))
  

(defn next-block []
  (dosync
    (alter (active-block :type) (fn [oldBlock] (random-block)))
    (alter (active-block :rotation) (fn [oldBlock] 0))
    (alter (active-block :rowIdx) (fn [oldBlock] 0))
    (alter (active-block :colIdx) (fn [oldBlock] 5))))

  
(defn first-non-zero-element [row]
  (count (take-while #(zero? %) row)))

(defn first-if [collection predicate]
  (count (take-while (fn [n] (not (predicate n))) collection)))
  
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

(defn max-y [b]
  (let [block-type  (deref (b :type))
        rotation    (deref (b :rotation))
        grids       (block-type :grids)
        grid-idx    (mod rotation (count grids))
        rows        (nth grids grid-idx) ]
    (- (dec (count rows)) (count (take-while (fn [row] (zero? (reduce max row))) (rseq rows))))))

(defn min-x [b]
  (let [block-type  (deref (b :type))
        rotation    (deref (b :rotation))
        grids       (block-type :grids)
        grid-idx    (mod rotation (count grids))
        rows        (nth grids grid-idx) ]
    (* -1 (first-non-zero-element
      (reduce (fn [r1 r2]
                (if (< (first-non-zero-element r1)
                       (first-non-zero-element r2))
  			      r1 r2))
              rows)))))

(defn move-left [b]
  (if (< (min-x b) (deref (b :colIdx)))
      (dosync (alter (b :colIdx) dec))))

(defn move-right [b]
  (if (< (+ (deref (b :colIdx)) (max-x b)) (dec (prefs :num-columns)))
    (dosync (alter (b :colIdx) inc))))

(defn commit-block [block]
  (let [  block-type  (deref (block :type))
          rowIdx   	  (deref (block :rowIdx))
          colIdx      (deref (block :colIdx))
          rotation    (deref (block :rotation))
          grids       (block-type :grids)
          grid-idx    (mod rotation (count grids))
          rows        (nth grids grid-idx)
          num-rows    (count rows)]
    (dotimes [ri num-rows]
      (let [current-row   (nth rows ri)
            num-columns   (count current-row)]
        (dotimes [ci num-columns]
          (let [cell-value (nth current-row ci)]
            (if-not (zero? cell-value)
              (let [c	(+ colIdx ci)
                    r	(+ rowIdx ri)]
                (set-field r c cell-value)))))))
    (next-block)))

(defn move-down [b]
  (if (< (+ (deref (b :rowIdx)) (max-y b)) (dec (prefs :num-rows)))
    (dosync (alter (b :rowIdx) inc))
    (commit-block b)))

(defn create-panel []
  (doto
    (proxy [JPanel KeyListener] []
      (paintComponent [g]
        (proxy-super paintComponent g)
        (draw-all g))
      (getPreferredSize [] (Dimension. (prefs :screen-width)
                                       (prefs :screen-height)))
      (keyPressed [e]
        (let [keyCode (.getKeyCode e)]          
          (.repaint this)
          (if (== 37 keyCode) (move-left active-block)
          (if (== 38 keyCode) (rotate active-block)
          (if (== 39 keyCode) (move-right active-block)
          (if (== 40 keyCode) (move-down active-block)
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
      ;(.repaint panel)
      (Thread/sleep 10)
      (recur))))

;(main)
