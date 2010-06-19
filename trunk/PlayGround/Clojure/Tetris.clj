(in-ns 'tetris)
(clojure.core/use 'clojure.core)
(import java.awt.Color)
(import java.awt.Dimension)
(import java.awt.Font)
(import java.awt.FontMetrics)
(import java.awt.geom.Rectangle2D)
(import java.awt.Toolkit)
(import java.util.Timer)
(import java.util.TimerTask)
(import java.awt.event.KeyListener)
(import javax.swing.JFrame)
(import javax.swing.JPanel)
(import javax.swing.UIManager)
(import javax.swing.JOptionPane)
(use 'clojure.contrib.math)

; =============================================================================
; HTTP Utility functions
; =============================================================================
(import java.net.URL)
(import java.io.InputStreamReader)
(import java.io.BufferedReader)
(import java.io.OutputStreamWriter)



(defn uri-encode [text]
  (.. #^String text
    (replace " " "%20")
    (replace "&" "%26")
    (replace "=" "%3D")
  ))
    
(defn uri-decode [text]
  (.. #^String text
    (replace "%20" " ")
    (replace "%26" "&")
    (replace "%3D" "=")
  ))
    
(defn do-get-request [url-string]
  (let [url           (URL. url-string)
        connection    (.openConnection url)
        is            (.getInputStream connection)
        isr           (InputStreamReader. is)
        in            (BufferedReader. isr)]
    (loop [line    (.readLine in)
           result  (str)]
      (if (not (nil? line))
        (do (recur (.readLine in) (str result line)))
        result))))

(defn do-post-request [url-string post-body]
    (println "POST body: " post-body)
    (let [url   (URL. url-string)
          conn  (do
                  (let [c (.openConnection url)]
                    (.setDoOutput c true)
                    (.setRequestMethod c "POST")
                    c))
          out   (OutputStreamWriter. (.getOutputStream conn))]
      (.write out post-body)
      (.close out)
      (println "POST is done")
      (let [in  (BufferedReader.
                  (InputStreamReader.
                    (.getInputStream conn)))]
        (loop [line   (.readLine in)
               result (str)]
          (if (not (nil? line))
            (do (recur (.readLine in) (str result line)))
            result)))))


; =============================================================================
; Grid data structure
; =============================================================================
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


; =============================================================================
; Shuffle utility function
; =============================================================================
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
  (sort-by-mem-keys (fn [_] (rand)) coll))


; =============================================================================
; Tetris
; =============================================================================
(def prefs {
  :field-x 1
  :field-y 1
  :num-rows 20
  :num-columns 10
  :block-width 16
  :block-height 16 })

(def window-side-width 200)
(def window-width (+ (* (+ 2 (prefs :num-columns)) (prefs :block-width)) window-side-width))
(def window-height (* (+ 2 (prefs :num-rows)) (prefs :block-height)))

(def i-block {
  :value 1
  :grids [  (create-grid-with-data 2 4 [ 0 1
                                         0 1
                                         0 1
                                         0 1])

            (create-grid-with-data 4 2 [ 0 0 0 0
                                         1 1 1 1])]})

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

(def bag-of-blocks (ref block-types)) ;(into block-types block-types)))

(def bag-index (ref 0))

(def next-blocks (ref []))
(def num-next-blocks 1)
(def is-game-over (atom false))
(def user-name (ref (str)))
(def hs-xml (ref nil))

(defn current-time-ms [] (.getTime (java.util.Date.)))
(def timer (ref (new Timer)))
(def frame-count (ref 0))
(def start-time (ref (current-time-ms)))
(def fps (ref 0))

; stats
(def stats {
  :score (ref 0)
  :lines (ref 0)
  :singles (ref 0)
  :doubles (ref 0)
  :triples (ref 0)
  :tetrises (ref 0)
  :i-blocks (ref 0)
  :j-blocks (ref 0)
  :l-blocks (ref 0)
  :o-blocks (ref 0)
  :s-blocks (ref 0)
  :t-blocks (ref 0)
  :z-blocks (ref 0)
})

(defn reset-stats []
  (alter (stats :score) (fn [_] 0))
  (alter (stats :lines) (fn [_] 0))
  (alter (stats :singles) (fn [_] 0))
  (alter (stats :doubles) (fn [_] 0))
  (alter (stats :triples) (fn [_] 0))
  (alter (stats :tetrises) (fn [_] 0))
  (alter (stats :i-blocks) (fn [_] 0))
  (alter (stats :j-blocks) (fn [_] 0))
  (alter (stats :l-blocks) (fn [_] 0))
  (alter (stats :o-blocks) (fn [_] 0))
  (alter (stats :s-blocks) (fn [_] 0))
  (alter (stats :t-blocks) (fn [_] 0))
  (alter (stats :z-blocks) (fn [_] 0)))


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
        (alter next-blocks
          (fn [_] (subvec (vec @bag-of-blocks) 1 (inc num-next-blocks))))))
    (alter bag-of-blocks
           (fn [_] (drop (inc num-next-blocks) @bag-of-blocks)))))

(declare check-position-valid)
(declare do-game-over)

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
    (if (= false @is-game-over)
      (do-game-over))))
  
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


(declare move-down)

(defn set-game-speed [interval]
  (let [task (proxy [TimerTask] []
               (run []
                (if (= false @is-game-over)
                  (move-down global-field active-block))))]
    (.cancel @timer)
    (alter timer (fn [_] (new Timer)))
    (.scheduleAtFixedRate @timer task (long interval) (long interval))))

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

(defn half [n] (round (int (/ n 2))))

(defn center-in-screen [frame]
  (let [  dim (.getScreenSize(Toolkit/getDefaultToolkit))
          w (.width (.getSize frame))
          h (.height (.getSize frame))
          x (half (- (.width dim) w))
          y 0 ;(half (- (.height dim) h))
          ]
  (.setLocation frame x y)))

(defn paint-grid-cell [graphics i j value]
  (let [w   (prefs :block-width)
        h   (prefs :block-height)          
        x   (+ (prefs :field-x) (* i w))
        y   (+ (prefs :field-y) (* j h)) ]
    (doto graphics
      (.setColor (get-color value))
      (.fillRect x y w h))))

(defn draw-text [g x y w h text]
  (doto g
    (.drawString text (* w x) (* h y))))

(defn draw-block [g block]
  (if (= false @is-game-over)
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
              (let [x  (+ (prefs :field-x) (+ bx i))
                    y  (+ (prefs :field-y) (+ by j))]
                (paint-grid-cell g x y cell-value)))))))))

(defn clear-screen [panel g]
  (doto g
    (.setColor (Color/GRAY))
    (.fillRect 0 0 (.getWidth panel) (.getHeight panel))))
                              
(defn draw-field [g field]
  (dotimes [x (field :width)]
    (dotimes [y (field :height)]
      (paint-grid-cell g (+ (prefs :field-x) x)
                         (+ (prefs :field-y) y)
                         (get-grid field x y)))))

(defn draw-debug-field [g field]
  (dotimes [x (field :width)]
    (dotimes [y (field :height)]
      (draw-text g (+ 12 x)
                 (+ (prefs :field-y) y)
                 (prefs :block-width)
                 (prefs :block-height)
                 (str (get-grid field x y))))))

(defn draw-next-blocks [g blocks]
  (if (= false @is-game-over)
    (dotimes [ n (count blocks) ]
      (let [ block      (nth blocks n)
             grids      (block :grids)
             grid-idx   0
             grid       (nth grids grid-idx) ]
        (dotimes [ i (grid :width) ]
          (dotimes [ j (grid :height) ]
            (let [ x      (+ i (+ (prefs :field-x) (prefs :num-columns) 1))
                   y      (+ 1 j (* n 5) (prefs :field-y))
                   value  (get-grid grid i j) ]
              (if-not (zero? value)
                (paint-grid-cell g x y value)))))))))

(defn debug-draw-bag-of-blocks [g blocks]
  (dotimes [ n (count blocks) ]
    (let [ block      (nth blocks n)
           grids      (block :grids)
           grid-idx   0
           grid       (nth grids grid-idx) ]
      (dotimes [ i (grid :width) ]
        (dotimes [ j (grid :height) ]
          (let [ x      (+ i (+ (prefs :field-x) (prefs :num-columns) 5))
                 y      (+ j (* n 5) (prefs :field-y))
                 value  (get-grid grid i j) ]
            (if-not (zero? value)
              (paint-grid-cell g x y 0))))))))

  
(defn calculate-fps []
  (let [current-time        (current-time-ms)
        time-interval-ms    (- (current-time-ms) @start-time)
        time-interval-s     (/ time-interval-ms 1000)
        time-interval       (if-not (zero? time-interval-s)
                              time-interval-s
                              1)]
    (dosync
      (alter fps (fn [_] (int (round (/ @frame-count time-interval)))))
      (alter frame-count (fn [_] 0))
      (alter start-time ( fn [_] (current-time-ms))))))

(defn draw-fps [g]
  (doto g
    (.setColor (Color/ORANGE))
    (.drawString (str "fps " @fps) 15 (* 22 15))))

(defn get-tetris-font [g]
  (.deriveFont (.getFont g)
               (int java.awt.Font/BOLD)
               (float 14)))
    
(defn get-text-rect [g str font]
  (.getStringBounds (.getFontMetrics g (get-tetris-font g)) str g))

(def text-line-height 20)

(defn draw-text-column [g x y lines]
  (dotimes [i           (count lines)]
    (let [  y           (+ y (* i text-line-height))
            line        (lines i)
            text        (line :text)
            color       (line :color)
            ; drawString draws a string that
            ; sits on top of it's y coord
            ; we prefer consistency with how rects are painted (below y)
            ; so we add the text height to the y value
            font-height (.getHeight (get-text-rect g text (.getFont g)))
            y-adjusted  (int (+ y font-height)) ]
    (.setColor g color)
    (.drawString g text x y-adjusted))))

(defn draw-left-aligned-text-column [g x y lines]
  (let [text-height (* (count lines) text-line-height)  ]
    (dotimes [i (count lines)]
      (let [ line        (lines i)
             text-rect   (get-text-rect g (line :text) (get-tetris-font g))
             text-width  (.getWidth text-rect) ]
        (.setColor g (line :color))
        (.drawString g (line :text) x (+ y (* i text-line-height)))))))

(defn draw-centered-text-column [g x y lines]
  (let [text-height (* (count lines) text-line-height)
        y-offset    (- y (half text-height)) ]
    (dotimes [i (count lines)]
      (let [ line        (lines i)
             text-rect   (get-text-rect g (line :text) (get-tetris-font g))
             text-width  (.getWidth text-rect)
             x-offset    (- x (half text-width)) ]
        (.setColor g (line :color))
        (.drawString g (line :text) x-offset (+ y-offset (* i text-line-height)))))))

(defn draw-stats [g]
  (let [lines     [ { :text (str "1x " @(stats :singles)) :color Color/LIGHT_GRAY }
                    { :text (str "2x " @(stats :doubles)) :color Color/LIGHT_GRAY }
                    { :text (str "3x " @(stats :triples)) :color Color/LIGHT_GRAY }
                    { :text (str "4x " @(stats :tetrises)) :color Color/LIGHT_GRAY }
                    { :text (str "total " @(stats :lines)) :color Color/LIGHT_GRAY }
                    { :text (str "score " @(stats :score)) :color Color/LIGHT_GRAY }
                    { :text (str "level " (get-level @(stats :lines)))
                      :color Color/LIGHT_GRAY } ]
        field-w   (* (prefs :block-width) (prefs :num-columns))
        field-h   (* (prefs :block-height) (prefs :num-rows))
        x-offset  (* (prefs :field-x) (prefs :block-width))
        y-offset  (* (prefs :field-y) (prefs :block-height))
        x         (+ x-offset field-w (prefs :block-width))
        y         (- (+ y-offset field-h) (* (count lines) text-line-height)) ]
    (.setFont g (get-tetris-font g))
    (draw-text-column g x y lines)))
    
(defn draw-high-scores [g hs-xml]
  (let [x-offset        (* (+ 1 (prefs :field-x)) (prefs :block-width))
        y-offset        (* (+ 2 (prefs :field-y)) (prefs :block-height))
        field-w         (* (prefs :block-width) (prefs :num-columns))
        field-h         (* (prefs :block-height) (prefs :num-rows))
        xml-entries     (hs-xml :content)
        num-entries     (count xml-entries)
        title           [ { :text "Hall of Fame" :color Color/ORANGE} ]
        hs-entries      (loop [i 0 result [] ]
                          (if (and (< i num-entries) (< i 10))
                            (let [xml-entry (nth xml-entries i)
                                  name      (uri-decode ((xml-entry :attrs) :name))
                                  score     ((xml-entry :attrs) :score)
                                  index     (if (= 0 i) "1ST"
                                            (if (= 1 i) "2ND"
                                            (if (= 2 i) "3RD"
                                            (if (= 3 i) "4TH"
                                            (if (= 4 i) "5TH"
                                            (if (= 5 i) "6TH"
                                            (if (= 6 i) "7TH"
                                            (if (= 7 i) "8TH"
                                            (if (= 8 i) "9TH"
                                            (if (= 9 i) "10TH" "INVALID"))))))))))
                                  text      (str name " " score) ]
                              (recur (inc i) (conj result {:text text :color Color/YELLOW})))
                            result)) ]
    (draw-left-aligned-text-column g x-offset y-offset (into title hs-entries))))
    
(defn draw-game-over [g]
  (let [x-offset        (* (prefs :field-x) (prefs :block-width))
        y-offset        (* (prefs :field-y) (prefs :block-height))
        field-w         (* (prefs :block-width) (prefs :num-columns))
        field-h         (* (prefs :block-height) (prefs :num-rows)) ]
    (draw-centered-text-column g (+ x-offset (half field-w))
                                 (+ y-offset (half field-h))
                                 [
                                  { :text "game over!"  :color Color/RED }
                                  { :text ""            :color Color/LIGHT_GRAY }
                                  { :text "press enter" :color Color/GREEN }
                                  { :text "to start"    :color Color/GREEN }
                                  { :text "new game"    :color Color/GREEN }
                                   ])))

(defn draw-all [panel g f b]
  (clear-screen panel g)
  (if (zero? (mod @frame-count 100))
    (calculate-fps))
  ;(draw-fps g)
  (draw-field g f)
  (draw-next-blocks g @next-blocks)
  (draw-block g b)
  (draw-stats g)
  (if (true? @is-game-over)
    (if-not (nil? @hs-xml)
      (draw-high-scores g @hs-xml))))

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

(defn update-score [num-lines-scored]
  (let [level   (get-level @(stats :lines))
        points  (if (== num-lines-scored 1) (* 40 (inc num-lines-scored))
                (if (== num-lines-scored 2) (* 100 (inc num-lines-scored))
                (if (== num-lines-scored 3) (* 300 (inc num-lines-scored))
                (if (== num-lines-scored 4) (* 1200 (inc num-lines-scored))
                (println "ERROR: num-lines-scored is: " num-lines-scored))))) ]
    (alter (stats :score) (fn [old-score] (+ old-score points)))))
      
      
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
        (alter (stats :lines) (fn [lines] (+ lines num-lines-scored)))
        (if (== num-lines-scored 1) (alter (stats :singles) inc)
        (if (== num-lines-scored 2) (alter (stats :doubles) inc)
        (if (== num-lines-scored 3) (alter (stats :triples) inc)
        (if (== num-lines-scored 4) (alter (stats :tetrises) inc)))))
        (update-score num-lines-scored))
      (set-game-speed (get-game-speed (get-level @(stats :lines)))))))

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

(defn reset-game []
  (reset! is-game-over false)
  (alter hs-xml (fn [_] nil))
  (reset-stats))

(defn create-panel []
  (doto
    (proxy [JPanel KeyListener] []
      (paintComponent [g]
        (proxy-super paintComponent g)
        (draw-all this g global-field active-block)
        (dosync (alter frame-count inc)))
      (getPreferredSize [] (Dimension. window-width
                                       window-height))
      (keyPressed [e]
        (let [keyCode (.getKeyCode e)]
          (if-not (true? @is-game-over)
            (if (== 37 keyCode) (move-left global-field active-block)
            (if (== 38 keyCode) (rotate global-field active-block)
            (if (== 39 keyCode) (move-right global-field active-block)
            (if (== 40 keyCode) (move-down global-field active-block)
            (if (== 32 keyCode) (drop-block global-field active-block))))))
          (if (== 10 keyCode)
              (dosync (reset-game))))))
      (keyReleased [e])
      (keyTyped [e]))
    (.setFocusable true)))

(defn main []
  (let [frame  (JFrame. "Tetris")
        panel  (create-panel)]
    (doto frame
      (.add panel)
      (.pack)
      (.setDefaultCloseOperation JFrame/EXIT_ON_CLOSE)    
      (.setVisible true))
    (.addKeyListener panel panel)
    (center-in-screen frame)
    (init-blocks)    
    (dosync
      (set-game-speed (get-game-speed (get-level @(stats :lines)))))
    (loop []
      (Thread/sleep 10)
      (.repaint panel)
      (recur))))
  
(defn get-user-name []
  (UIManager/setLookAndFeel (UIManager/getSystemLookAndFeelClassName))
  (JOptionPane/showInputDialog nil "What is your name?" "Name" JOptionPane/QUESTION_MESSAGE))
      
(def domain "stacked-crooked.com")
;(def domain "localhost")

(defn do-game-over []
  (if-not (= true @is-game-over)
    (do
      (reset! is-game-over true)
      (alter user-name (fn [_] (get-user-name)))
      (if-not (nil? @user-name)
        (let [url         (str "http://" domain "/hs")
              post-body   (str "name=" (uri-encode @user-name) "&score=" @(stats :score))]
          (println "Post request" (do-post-request url post-body))))
      (alter hs-xml (fn [_] (clojure.xml/parse (str "http://" domain "/hof.xml"))))
      (init-blocks))))

; TODO
; ----
; - high-score (internet?)
; v lines
; v level
; v show score http://tetris.wikia.com/wiki/Scoring
; v game over message
; v drop block space bar
;(main)
