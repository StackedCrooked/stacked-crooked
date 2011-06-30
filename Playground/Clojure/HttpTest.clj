(import java.net.URL)
(import java.io.InputStreamReader)
(import java.io.BufferedReader)
(import java.io.OutputStreamWriter)


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
    (let [url   (URL. url-string)
          conn  (do
                  (let [c (.openConnection url)]
                    (.setDoOutput c true)
                    (.setRequestMethod c "POST")
                    c))
          out   (OutputStreamWriter. (.getOutputStream conn))]
      (.write out post-body)
      (.close out)
      (let [in  (BufferedReader.
                  (InputStreamReader.
                    (.getInputStream conn)))]
        (loop [line   (.readLine in)
               result (str)]
          (if (not (nil? line))
            (do (recur (.readLine in) (str result line)))
            result)))))

; Example of a GET request
;(println (do-get-request "http://www.google.com"))
            
; Example of a POST request
; (println (do-post-request "http://localhost/hs" "name=Clojure&score=42"))

