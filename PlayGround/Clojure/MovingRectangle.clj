(import java.awt.Color)
(import java.awt.Dimension)
(import java.awt.Toolkit)
(import java.awt.event.KeyListener)
(import javax.swing.JFrame)
(import javax.swing.JPanel)

(def view-prefs {
  :block-width 10
  :block-height 10
  :screen-width 320
  :screen-height 240
})

(defn center-in-screen [frame]
  (let [dim (.getScreenSize(Toolkit/getDefaultToolkit))
       w (.width (.getSize frame))
       h (.height (.getSize frame))
       x (int (/ (- (.width dim) w) 2))
       y (int (* 0.75(/ (- (.height dim) h) 2)))]
  (.setLocation frame x y)))

(defn drawRectangle [p x y w h]  
  (doto (.getGraphics p)
    (.setColor (java.awt.Color/BLACK))
    (.fillRect 0 0 (view-prefs :screen-width) (view-prefs :screen-height))
    (.setColor (java.awt.Color/BLUE))
    (.fillRect (* w (deref x)) (* h (deref y)) w h)))
    
(defn drawGameState [panel gs]
  (drawRectangle panel
                 ((gs :block) :x)
                 ((gs :block) :y)
                 (view-prefs :block-width)
                 (view-prefs :block-height)))

(defn create-panel [x y]
  (doto
    (proxy [JPanel KeyListener] []
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
        panel (create-panel ((gamestate :block) :x)
                            ((gamestate :block) :y))
        ]
    (doto frame
      (.add panel)
      (.pack)
      (.setDefaultCloseOperation JFrame/EXIT_ON_CLOSE)    
      (.setVisible true))
    (.addKeyListener panel panel)
    (center-in-screen frame)
    (loop []
      (drawGameState panel gamestate)
      (Thread/sleep 10)
      (recur))))
(main)
