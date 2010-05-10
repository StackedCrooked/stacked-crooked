(import java.awt.Color)
(import java.awt.Dimension)
(import java.awt.Toolkit)
(import java.awt.event.KeyListener)
(import javax.swing.JFrame)
(import javax.swing.JPanel)

(def prefs {
  :block-width 10
  :block-height 10
  :screen-width 320
  :screen-height 240
})

(defn centerInScreen [frame]
  (let [  dim (.getScreenSize(Toolkit/getDefaultToolkit))
          w (.width (.getSize frame))
          h (.height (.getSize frame))
          x (int (/ (- (.width dim) w) 2))
          y (int (* 0.75(/ (- (.height dim) h) 2)))]
  (.setLocation frame x y)))

(defn drawRectangle [g x y w h]
  (doto g
    (.setColor (java.awt.Color/BLACK))
    (.fillRect 0 0 (prefs :screen-width) (prefs :screen-height))
    (.setColor (java.awt.Color/BLUE))
    (.fillRect (* w (deref x)) (* h (deref y)) w h)))
    
(defn drawGameState [gs g]
  (drawRectangle g
                 ((gs :block) :x)
                 ((gs :block) :y)
                 (prefs :block-width)
                 (prefs :block-height)))

(defn createPanel [gs x y]
  (doto
    (proxy [JPanel KeyListener] []
      (paintComponent [g]
        (proxy-super paintComponent g)
        (drawGameState gs g))
      (getPreferredSize [] (Dimension. 320 240))
      (keyPressed [e]
        (let [keyCode (.getKeyCode e)]
          (if (== 37 keyCode) (dosync (alter x dec))
          (if (== 38 keyCode) (dosync (alter y dec))
          (if (== 39 keyCode) (dosync (alter x inc))
          (if (== 40 keyCode) (dosync (alter y inc))
                              (println keyCode)))))))
      (keyReleased [e])
      (keyTyped [e]))
    (.setFocusable true)))

(defn main []
  (let [gamestate {
          :field {
            :buffer (make-array Integer 200)
            :width 10
            :height 20
          }
          :block {
            :type (ref :I)
            :rotation 0
            :x (ref 0)
            :y (ref 0)
          }
        }
        frame (JFrame. "Test")
        panel (createPanel gamestate
                            ((gamestate :block) :x)
                            ((gamestate :block) :y))
        ]
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
(main)
