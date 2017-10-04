import TType
import TExpr
import Control.Monad.State


mty = makeTType [20, 30]
uty = makeTType [30, 30, 30]
vty = makeTType [20, 20, 20]
a = tensor "A" mty
b = tensor "B" mty
c = tensor "C" mty
u = tensor "u" uty
v = tensor "v" vty
expr = contr a 1 (contr b 1 (contr c 1 (trans u 0 2) 2) 2) 2
s = tensor "s" []
expr' =  expr # expr
expr'' = expr' .+ expr

main = do
  let Just e = expr'
      e' = buildTExprGraph e
      --dmap = getDepthMap e'
      --kmap = getTKindMap e'
  putStrLn . show . printTExpr $ e
  let (as, e'') = replaceContracts e'
  foldM (\_ -> \(lbl, t) ->
          let prettyExpr = printTExpr t
              assignment = lbl ++ " = " ++ (show prettyExpr)
          in do
            putStrLn assignment
            let imap = fst $ runState (getIndexMap t) 0
            putStrLn . show $ (bottomUpCodeGen lbl imap t)
            )
        ()
        as
  putStrLn $ "result = " ++ (show $ printTExpr e'')
  let imap = fst $ runState (getIndexMap e'') 0
  putStrLn . show $ (bottomUpCodeGen "result" imap e'')
  
      
-- main =
  -- let mty = makeTType [20, 20]
      -- uty = makeTType [20, 20, 20]
      -- vty = makeTType [20, 20, 20]
      -- a = makeTTensor "A" mty
      -- b = makeTTensor "B" mty
      -- c = makeTTensor "C" mty
      -- u = makeTTensor "u" uty
      -- v = makeTTensor "v" vty
      -- expr = contr a 1 (contr b 1 (contr c 1 u 2) 2) 2
      -- s = makeTTensor "s" []
      -- expr' =  s .* expr
      -- Just expr'' = expr' .+ s.* trans expr 0 1
  -- -- in putStrLn . show $ typeCheck (s .+ v) expr
      -- e = (s .* (a .+ a ./ a) # u ./ s)
  -- in do putStrLn . printTExpr $ expr''
        -- --putStrLn . show $ getContractions expr''
        -- foldM (\_ -> \(a,b) -> do
                -- putStr $ show b ++ " -- "
                -- putStrLn . printTExpr $ a) () $ getContrWithDepths expr''
                
        -- let (p, t) = replaceContractions expr''
        -- foldM (\_ -> \(lbl, t) -> putStrLn $ lbl ++ " = " ++ printTExpr t) () p
        -- putStrLn . printTExpr $ t
        
        -- let a = ("u", expr'')
            -- as = replaceIncompatible a
        -- foldM (\_ -> \(lbl, t) -> putStrLn $ lbl ++ " = " ++ printTExpr t) () as
        
        -- putStrLn . printTExpr $ uniquify expr''
