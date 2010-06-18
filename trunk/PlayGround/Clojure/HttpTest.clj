(import java.net.URL)
(import java.io.InputStreamReader)
(import java.io.BufferedReader)
(defn get-stream [url-string]
    (let [url           (URL. url-string)
          connection    (.openConnection url)
          is            (.getInputStream connection)
          isr           (InputStreamReader. is)
          in            (BufferedReader. isr)]
        in))
(defn get-url [url-string]
    (let [in      (get-stream url-string)]
        (loop [line (.readLine in)
               result (str)]
            (if (not (nil? line))
                (do (recur (.readLine in) (str result line)))
                result))))

; Print Google homepage HTML
(println (get-url "http://www.google.com"))
