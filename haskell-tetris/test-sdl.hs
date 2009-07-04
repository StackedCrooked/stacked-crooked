import Prelude
import Graphics.UI.SDL as SDL

background = "background.bmp"
hello = "hello_world.bmp"

main = do
  SDL.init [InitEverything]
  setVideoMode 640 480 32 []
  image <- loadBMP hello
  back <- loadBMP background
  screen <- getVideoSurface
  blitSurface back Nothing screen Nothing
  blitSurface image Nothing screen (Just (Rect 180 140 0 0))
  SDL.flip screen
  quitHandler

quitHandler :: IO ()
quitHandler = do
  e <- waitEvent
  case e of
    Quit -> return ()
    otherwise -> quitHandler