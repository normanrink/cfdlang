module TExpr (
  TExpr(),
  getTKind, getTType, getTID, getTLabel,
  tensor, (.+), (.-), (.*), (./), (#), contr, trans,
  buildTExpr, buildTExprGraph,
  printTExpr,
  addDepths, getContracts,
  getDepthMap, getTKindMap, getMaxDepthContract, replaceContracts,
  getIndexMap,
  emitForHeader, emitForFooter,
  bottomUpCodeGen
  ) where

import TreeUtil
import TType

import Control.Monad.State
import Data.AssocList
import qualified Data.List.Extras as Extras
import qualified Data.Map as Map
import Data.Tree
import Text.PrettyPrint

   
data TExprKind = TK_Tensor
               | TK_Add
               | TK_Sub
               | TK_Mul
               | TK_Div
               | TK_Product
               | TK_Transpose
               | TK_Contract
               deriving (Eq, Ord, Show)

getTKString :: TExprKind -> String
getTKString TK_Tensor = "TTensor"
getTKString TK_Add = "TAdd"
getTKString TK_Sub = "TSub"
getTKString TK_Mul = "TMul"
getTKString TK_Div = "TDiv"
getTKString TK_Product = "TProduct"
getTKString TK_Transpose = "TTranspose"
getTKString TK_Contract = "TContract"

getTOp :: TExprKind -> String
getTOp TK_Add = " + "
getTOp TK_Sub = " - "
getTOp TK_Mul = " * "
getTOp TK_Div = " / "
getTOp TK_Product = " * "
getTOp _ = undefined

data TExprInfo a = TExprInfo { getKind :: TExprKind
                             , getType :: TType
                             , getID :: a 
                 -- Only needed for 'TK_Tensor':
                             , getLabel :: String
                 -- Only needed for 'TK_Transpose' and 'TK_Contract':
                             , getIndices :: [TIdx] 
                             }
                 deriving (Eq, Ord, Show)
                 

type TExpr a = Tree (TExprInfo a)

getInfo :: TExpr a -> TExprInfo a
getInfo (Node info _) = info

getTKind :: TExpr a -> TExprKind
getTKind = getKind . getInfo

getTType :: TExpr a -> TType
getTType = getType . getInfo

getTID :: TExpr a -> a
getTID = getID . getInfo

getTLabel :: TExpr a -> String
getTLabel = getLabel . getInfo

getTIndices :: TExpr a -> [TIdx]
getTIndices = getIndices . getInfo

  
type M_TExpr a = Maybe (TExpr a)

makeTTensor :: String -> TType -> a -> M_TExpr a
makeTTensor lbl ty a = 
  let info = TExprInfo TK_Tensor ty a lbl [] 
  in Just $ Node info []
  
makeTAdd :: TExpr a -> TExpr a -> a -> M_TExpr a
makeTAdd t0 t1 a = 
  let ty0 = getType $ getInfo t0
      ty1 = getType $ getInfo t1
      info = TExprInfo TK_Add ty0 a "" []
  in if ty0 == ty1
        then Just $ Node info [t0, t1]
        else Nothing

makeTSub :: TExpr a -> TExpr a -> a -> M_TExpr a
makeTSub t0 t1 a = 
  let ty0 = getType $ getInfo t0
      ty1 = getType $ getInfo t1
      info = TExprInfo TK_Sub ty0 a "" []
  in if ty0 == ty1
        then Just $ Node info [t0, t1]
        else Nothing

makeTMul :: TExpr a -> TExpr a -> a -> M_TExpr a
makeTMul t0 t1 a = 
  let ty0 = getType $ getInfo t0
      ty1 = getType $ getInfo t1
      info = TExprInfo TK_Mul ty1 a "" []
  in if (isScalarTType ty0) || (ty0 == ty1)
        then Just $ Node info [t0, t1]
        else Nothing

makeTDiv :: TExpr a -> TExpr a -> a -> M_TExpr a
makeTDiv t0 t1 a =  
  let ty0 = getType $ getInfo t0
      ty1 = getType $ getInfo t1
      info = TExprInfo TK_Mul ty0 a "" []
  in if (isScalarTType ty1) || (ty0 == ty1)
        then Just $ Node info [t0, t1]
        else Nothing

makeTProduct :: TExpr a -> TExpr a -> a -> M_TExpr a
makeTProduct t0 t1 a = 
  let ty0 = getType $ getInfo t0
      ty1 = getType $ getInfo t1
      tyP = makeProductTType ty0 ty1
      info = TExprInfo TK_Product tyP a "" []
  in Just $ Node info [t0, t1]

makeTTranspose :: TExpr a -> TIdx -> TIdx -> a -> M_TExpr a
makeTTranspose t i0 i1 a = 
  let ty  = getType $ getInfo t
      tyT = makeTransposedTType ty i0 i1
  in if (i0 < rank ty) && (i1 < rank ty)
        then let info = TExprInfo TK_Transpose tyT a "" [i0, i1]
             in Just $ Node info [t]
        else Nothing

makeTContract :: TExpr a -> TIdx -> TExpr a -> TIdx -> a -> M_TExpr a
makeTContract t0 i0 t1 i1 a = 
  let ty0 = getType $ getInfo t0
      ty1 = getType $ getInfo t1
      tyC = makeContractionTType ty0 i0 ty1 i1
  in if (i0 < rank ty0) && (i1 < rank ty1) && (dim ty0 i0 == dim ty1 i1)
        then let info = TExprInfo TK_Contract tyC a "" [i0, i1]
             in Just $ Node info [t0, t1]
        else Nothing


tensor :: String -> TType -> M_TExpr ()
tensor lbl ty = makeTTensor lbl ty ()

infixl 6 .+
(.+) :: M_TExpr () -> M_TExpr () -> M_TExpr ()
(.+) t0 t1 = do
  t0' <- t0
  t1' <- t1
  makeTAdd t0' t1' ()

infixl 6 .-
(.-) :: M_TExpr () -> M_TExpr () -> M_TExpr ()
(.-) t0 t1 = do
  t0' <- t0
  t1' <- t1
  makeTSub t0' t1' ()

infixl 7 .*
(.*) :: M_TExpr () -> M_TExpr () -> M_TExpr ()
(.*) t0 t1 = do
  t0' <- t0
  t1' <- t1
  makeTMul t0' t1' ()

infixl 7 ./
(./) :: M_TExpr () -> M_TExpr () -> M_TExpr ()
(./) t0 t1 = do
  t0' <- t0
  t1' <- t1
  makeTDiv t0' t1' ()

infixl 8 #
(#) :: M_TExpr () -> M_TExpr () -> M_TExpr ()
(#) t0 t1 = do
  t0' <- t0
  t1' <- t1
  makeTProduct t0' t1' ()

contr :: M_TExpr () -> TIdx -> M_TExpr () -> TIdx -> M_TExpr ()
contr t0 i0 t1 i1 = do
  t0' <- t0
  t1' <- t1
  makeTContract t0' i0 t1' i1 ()

trans :: M_TExpr () -> TIdx -> TIdx -> M_TExpr ()
trans t i0 i1 = do
  t' <- t
  makeTTranspose t' i0 i1 ()
 

-- assign unique identifiers to each node in an expression tree:
type TExprID = Int

buildTExpr :: TExpr a -> TExpr TExprID
buildTExpr t = 
  fst $ runState (traverseS Post RtoL strafo dtrafo t) 0
    where strafo i _    = i + 1
          dtrafo i info =
            let (TExprInfo k ty a lbl inds) = info
            in TExprInfo k ty i lbl inds

-- keep identical nodes identical; this amounts to
-- operating on a graph (rather than a tree):
buildTExprGraph :: Eq a => TExpr a -> TExpr TExprID
buildTExprGraph t = 
  fst $ runState (buildTExprGraph' t) (0, [])

type TExprAssoc a = AssocList (TExpr a) [TExprID]

buildTExprGraph' :: Eq a => TExpr a -> State (Int, TExprAssoc a) (TExpr TExprID)
buildTExprGraph' t =
  let (TExprInfo k ty a lbl inds) = getInfo t
      sts = subForest t
  in do
    sts' <- foldM (\sts -> \t -> do
                                  t' <- buildTExprGraph' t
                                  return $ sts ++ [t'])
                  []
                  sts
    (i, map) <- get
    if hasEntry t map
      then let [i']  = lookup1 t map 
               info' = TExprInfo k ty i' lbl inds
           in return $ Node info' sts'
      else let i'    = i + 1
               info' = TExprInfo k ty i' lbl inds
               map'  = addEntry t [i'] map
           in do
                put (i', map')
                return $ Node info' sts'


-- get the expression node with a specific identifier:
getTExprWithID :: TExpr TExprID -> TExprID -> TExpr TExprID
getTExprWithID t i = 
  let ts = getTExprWithID' t i
      1 = length ts
  in head ts
 
getTExprWithID' :: TExpr TExprID -> TExprID -> [TExpr TExprID]
getTExprWithID' t i =
  if i == getTID t
     then [t]
     else concat $ map (\t -> getTExprWithID' t i) (subForest t) 


-- pretty printing of expression trees:
printTExpr :: (Show a) => TExpr a -> Doc
printTExpr = prettyPrint 0

prettyPrint :: (Show a) => Int -> TExpr a -> Doc
prettyPrint indent t@(Node info _) =
  let kind    = getTKString $ getKind info
      a       = show $ getID info
      lbl     = getLabel info
      ty      = show $ getType info
      indices = show $ getIndices info
      indent' = (length kind) + 1
      doc = (text kind) <> space <>
            braces (text $ "ID: " ++ a) <> space <>
            braces (text $ "Type: " ++ ty) <> space <>
            braces (text $ "Label: " ++ lbl) <> space <>
            braces (text $ "Indices: " ++ indices) <> space
      doc' = (foldl (\d -> \st -> d $$ prettyPrint indent' st)
                    doc
                    (subForest t))
  in nest indent $ parens doc'


type TExprMap a = Map.Map TExprID a

-- assign depths to the nodes in an expression tree:
type TDepth = Int

addDepths :: TExpr a -> TExpr (a, TDepth)
addDepths = addDepths' 0

addDepths' :: TDepth -> TExpr a -> TExpr (a, TDepth)
addDepths' d t =
  let (TExprInfo k ty a lbl inds) = getInfo t
      info' = TExprInfo k ty (a,d) lbl inds
      subForest' = foldr (\t -> \ts -> (addDepths' (d+1) t):ts)
                         []
                         (subForest t)
  in Node info' subForest'
  
getDepthMap :: TExpr TExprID -> TExprMap TDepth
getDepthMap t =
  let td = addDepths t
      dtrafo map = id
      strafo map (TExprInfo _ _ (i,d) _ _) = Map.insert i d map
  in snd $ runState (traverseS Pre LtoR strafo dtrafo td) Map.empty

getMaxDepthMap :: TExpr TExprID -> TExprMap TDepth
getMaxDepthMap t =
  let td = addDepths t
      dtrafo map = id
      -- if identical expression nodes are allowed in an expression
      -- tree, then keep the maximum depth corresponding to the node:
      strafo map (TExprInfo _ _ (i,d) _ _) = Map.insertWith max i d map
  in snd $ runState (traverseS Pre LtoR strafo dtrafo td) Map.empty
  
-- map nodes in expression trees to their kinds:
getTKindMap :: TExpr TExprID -> TExprMap TExprKind
getTKindMap t =
  let dtrafo map = id
      strafo map (TExprInfo k _ i _ _) = Map.insert i k map
  in snd $ runState (traverseS Pre LtoR strafo dtrafo t) Map.empty


-- get the most deeply nested contraction in an expression tree:
getMaxDepthContract' :: TExprMap TDepth -> TExprMap TExprKind -> TExprID
getMaxDepthContract' dmap kmap =
-- use list comprehensions:
  let cs  = [ c | (c,k) <- Map.toList kmap, k == TK_Contract ] 
      ids = [ (i,d) | (i,d) <- Map.toList dmap, i `elem` cs ]
      (i,_) = Extras.argmax snd ids
  in i

getMaxDepthContract :: TExpr TExprID -> TExpr TExprID
getMaxDepthContract t =
  let kmap = getTKindMap t
      dmap = getDepthMap t
      i = getMaxDepthContract' dmap kmap
  in getTExprWithID t i

-- find all contractions in an expression tree:
getContracts :: TExpr a -> [a]
getContracts t = snd $ runState (traverseS Pre LtoR strafo dtrafo t) []
  where dtrafo s = id
        strafo s info = case getKind info of
                          TK_Contract -> (getID info):s
                          _ -> s


type TAssignment a = (String, TExpr a)

-- recursively replace contractions until there is at most
-- a single contraction left in all expression trees:
replaceContracts :: TExpr TExprID -> ([TAssignment TExprID], TExpr TExprID)
replaceContracts t =
  let (t', (_, as)) = runState (replaceContracts' t) (0, [])
  in (as, t')

replaceContracts' :: TExpr TExprID ->
                     State (Int, [TAssignment TExprID]) (TExpr TExprID)
replaceContracts' t =
  if length (getContracts t) <= 1
     then return t
     else do
      (i, as) <- get
      let c = getMaxDepthContract t
          lbl = "tmp" ++ show i
          Just r = makeTTensor lbl (getTType c) (-1)
          i' = i + 1
          t' = buildTExpr $ replaceContractWith t c r
      put (i', as ++ [(lbl, c)])
      replaceContracts' t'
          
replaceContractWith :: (Eq a) => TExpr a -> TExpr a -> TExpr a -> TExpr a
replaceContractWith t contraction replacement =
  if t == contraction
     then replacement
     else let sts  = subForest t
              sts' = map (\t -> replaceContractWith t contraction replacement)
                         (subForest t)
          in Node (getInfo t) sts'


-- TODO:
-- lift tensors out of expression trees if there is an incompatible assignment

-- assign indices to the nodes of an expression tree;
-- every node is assigned as many indices as the rank of its type:
addIndices :: TExpr a -> State Int (TExpr (a, [String]))
addIndices t = do
  i <- get
  let rk = rank $ getTType t
      indices = map (\i -> "i" ++ show i) $ take rk [i..]
  put (i + rk)
  addIndices' indices t

addIndices' :: [String] -> TExpr a -> State Int (TExpr (a, [String]))
addIndices' indices t = do
  let (TExprInfo k ty a lbl inds) = getInfo t
      subtrees = subForest t
      info' = TExprInfo k ty (a, indices) lbl inds
  case k of
    TK_Tensor -> do
      let [] = subtrees
      return $ Node info' []
    TK_Product -> do
      let [t0, t1] = subtrees
          rk0 = rank $ getTType t0
          indices0 = take rk0 indices
          indices1 = drop rk0 indices
      t0' <- addIndices' indices0 t0
      t1' <- addIndices' indices1 t1
      return $ Node info' [t0', t1']
    TK_Transpose -> do
      let [t0] = subtrees
          [i0, i1] = inds
          indices' = if i0 == i1
                        then indices'
                        else let i0' = min i0 i1
                                 i1' = max i0 i1
                                 ind0 = indices !! i0'
                                 ind1 = indices !! i1'
                                 head = take i0' indices
                                 tail = drop (i0'+1) indices
                                 middle = take (i1'-i0'-1) tail
                                 end = drop (i1'-i0') tail
                             in head ++ [ind1] ++ middle ++ [ind0] ++ end
      t0' <- addIndices' indices' t0
      return $ Node info' [t0']
    TK_Contract -> do
      i <- get
      let [t0, t1] = subtrees
          [i0, i1] = inds
          indc = "i" ++ show i ++ "_contr"
          rk0 = rank $ getTType t0
          indices0 = take (rk0-1) indices
          indices1 = drop (rk0-1) indices
          indices0' = (take i0 indices0) ++ [indc] ++
                      (drop i0 indices0)
          indices1' = (take i1 indices1) ++ [indc] ++
                      (drop i1 indices1)
      put (i + 1)
      t0' <- addIndices' indices0' t0
      t1' <- addIndices' indices1' t1
      return $ Node info' [t0', t1']
    _ -> do
      let [t0, t1] = subtrees
          rk0 = rank $ getTType t0
          rk1 = rank $ getTType t1
          indices0 = take rk0 indices
          indices1 = take rk1 indices
      t0' <- addIndices' indices0 t0
      t1' <- addIndices' indices1 t1
      return $ Node info' [t0', t1']

getIndexMap :: TExpr TExprID -> State Int (TExprMap [String])
getIndexMap t = do
  tis <- addIndices t
  let dtrafo map = id
      strafo map (TExprInfo _ _ (i,is) _ _) = Map.insert i is map
  return $ snd $ runState (traverseS Pre LtoR strafo dtrafo tis) Map.empty
  

getSubscript :: [String] -> String
getSubscript = foldl (\s -> \i -> s ++ "[" ++ i ++ "]") ""

getBinOp :: TExpr a -> String
getBinOp = getTOp . getTKind


type Nesting = (Int, [String])

emitForHeader :: [String] -> TType -> State Nesting Doc
emitForHeader indices ty =
  if length indices > 0
     then do
        (indent, loopedIndices) <- get
        let index = head indices
            bound = show $ head ty
            doc   = text $ "for (int " ++ index ++ " = 0; " ++
                                 index ++ " < " ++ bound ++ "; " ++
                                 index ++ "++) {"
        put (indent + 2, index : loopedIndices)
        doc' <- emitForHeader (tail indices) (tail ty)
        return $ (nest indent doc) $$ doc'    
     else return empty
     
emitForFooter :: [String] -> State Nesting Doc
emitForFooter indices = do
  if length indices > 0
     then do
        (indent, loopedIndices) <- get
        put (indent - 2, tail loopedIndices)
        doc' <- emitForFooter (tail indices)
        return $ (nest indent $ text "}") $$ doc'
     else return empty

emitTempDecl :: String -> State Nesting Doc
emitTempDecl temp = do
  let decl = "double " ++ temp ++ " = 0.0;"
  (indent, _) <- get
  return $ nest indent (text decl)
  
   
data Emission = Emission { getIndent :: Int
                         , getCounter :: Int
                         , getDoc :: Doc }
                
bottomUpCodeGen :: String -> TExprMap [String] -> TExpr TExprID -> Doc
bottomUpCodeGen target imap t =
  -- TODO: handle TK_Product so that left loop nest is separate from right
  -- loop nest.
  let ty  = getTType t
      Just indices = Map.lookup (getTID t) imap
      (header, (indent,_)) = runState (emitForHeader indices ty)
                                      (0, [])
      (result, em)         = runState (bottomUpCodeGen' imap t)
                                      (Emission indent 0 empty)
      (footer, (_,_))      = runState (emitForFooter indices)
                                      (indent-2, [])
  in header $$
     getDoc em $$
     nest indent (text $ target ++ (getSubscript indices) ++
                         " = " ++
                         result ++ ";") $$
     footer
  
bottomUpCodeGen' :: TExprMap [String] -> TExpr TExprID ->
                    State Emission String
bottomUpCodeGen' imap t = do
  (Emission indent counter doc) <- get
  let TExprInfo k ty i lbl inds = getInfo t
      Just indices = Map.lookup i imap
  case k of
    TK_Tensor -> return $ lbl ++ getSubscript indices
    TK_Contract ->
      let [t0, t1] = subForest t
          [i0, i1] = inds
          Just indices0 = Map.lookup (getTID t0) imap
          ci     = indices0 !! i0
          bound  = dim (getTType t0) i0
          temp   = "t" ++ show counter
          (decl, (_,_))         = runState (emitTempDecl temp) (indent, []) 
          (header, (indent',_)) = runState (emitForHeader [ci] [bound])
                                           (indent, [])
          (footer, (_,_))       = runState (emitForFooter [ci])
                                           (indent'-2, [])
      in do put (Emission (indent+2) (counter+1) (decl $$ header))
            r0 <- bottomUpCodeGen' imap t0
            r1 <- bottomUpCodeGen' imap t1
            (Emission indent'' counter' doc') <- get
            let doc'' = doc' $$
                        nest indent'' (text $ temp ++
                                              " += " ++
                                              r0 ++ " * " ++ r1 ++ ";") $$
                        footer
            put (Emission indent'' counter' doc'')
            return $ temp
    TK_Transpose -> let [t0] = subForest t
                    in bottomUpCodeGen' imap t0
    -- binary operations:
    -- TODO: handle TK_Product so that left loop nest is separate from right
    -- loop nest.
    _ -> let op = getBinOp t
             [t0, t1] = subForest t
         in do r0 <- bottomUpCodeGen' imap t0
               r1 <- bottomUpCodeGen' imap t1
               return $ r0 ++ op ++ r1

      
-- emitContraction :: TExprMap [String] -> String -> TExpr TExprID ->
                   -- State Nesting Doc
-- emitContraction imap target t = do
  -- (indent, loopedIndices) <- get
  -- let (TExprInfo TK_Contract ty i lbl inds) = getInfo t
      -- [t0, t1] = subForest t
      -- [i0, i1] = inds
      -- Just indices  = Map.lookup i imap
      -- Just indices0 = Map.lookup (getTID t0) imap
      -- ci = indices0 !! i0
      -- toLoopIndices = [ i | i <- indices, i `notElem` loopedIndices ]
      
  -- header <- emitForHeader toLoopIndices
  -- init = text $ target ++ getSubscript indices ++ " = 0.0;"
  -- body <- do
            -- emitForHeader ci
            -- text $ target += 
            -- emitForFooter ci
  -- footer <- emitForFooter toLoopIndices
  -- return $ (header $$ init $$ body $$ footer)

                      
    

-- genCode :: Bool -> String -> TExpr TExprID -> TEpxrMap [String] -> Doc
-- genCode False target t imap = genCode' 0 target t imap
-- genCode True  target t imap =
  -- let size = size $ getTType t
      -- decl = (text $ "double " ++ target ++ "[" ++ show size ++ "];")
  -- in decl $$ genCode' 0 target t imap

-- getSubscript :: [String] -> String
-- getSubscript = foldl (\s -> \i -> s ++ "[" ++ i ++ "]") ""

-- getBinOpString :: TExpr a -> String
-- getBinOpString t | getTKind t == TK_Add = " + "
                 -- | getTKind t == TK_Sub = " - "
                 -- | getTKind t == TK_Mul = " * "
                 -- | getTKind t == TK_Div = " / "
                 -- | getTKind t == TK_Product = " * "
                 -- | otherwise = undefined
 
-- getTmp :: Int -> String
-- getTmp i = "t" ++ show i

-- getTmpDecl :: String -> Doc
-- getTmpDecl tmp = text $ "double " ++ tmp ++ " = 0.0;"

-- getBinOpCode :: Int -> TExprMap [String] -> TExpr TExprID ->
                -- State Int (String, Doc)
-- getBinOpCode indent imap t =
  -- i <- get
  -- let tmp0     = getTmp i
      -- tmp1     = getTmp (i+1)
      -- op       = getBinOpString t
      -- [t0, t1] = subForest t
  -- put (i+2)
  -- (res0, doc0) <- getCode' indent imap tmp0 t0
  -- (res1, doc1) <- getCode' indent imap tmp1 t1
  -- let doc = doc0 $$ doc1 $$ 
  
  
-- genContract :: Int -> TAssignment -> TExprMap [String] -> State Int Doc
-- genContract indent (target, t) imap
  -- | getTKind t == TK_Contract = do
      -- i <- get
      -- let tmp0     = getTmp i
          -- tmp1     = getTmp (i+1
          -- [t0, t1] = subForest t
      -- put (i+2)
      -- doc0 <- genCode' (indent+2) (tmp0, t0) imap
      -- doc1 <- genCode' (indent+2) (tmp1, t1) imap
      -- let doc' = getTmpDecl tmp0 $$
                 -- getTmpDecl tmp1 $$
                 -- doc0 $$
                 -- doc1
                 
                 
      
      
  -- | otherwise = undefined
  

-- genCode' :: Int -> TAssignment -> TExprMap [String] -> State Int Doc
-- genCode' indent imap as@(target, t) =
  -- | getTKind t == TK_Contract = genContract indent as imap
  -- | getTKind t == TK_Tensor =
  -- -- TODO:
  -- | getTKind t == TK_Transpose = undefined
  -- -- TODO, binary operation:
  -- | otherwise = undefined
  
  -- i <- get
  -- let (TExprInfo k ty i lbl inds) = getInfo t
      -- subtrees     = subForest t
      -- Just indices = Map.lookup i imap
      -- subscript    = getSubscript indices
      -- indent'      = indent + 2
      -- doc = case k of
        -- TK_Tensor -> let [] = subtrees
                     -- in text $ lbl ++ subscript
        -- TK_Contract -> let [t0, t1] = subtrees text
                           -- [i0, i1] = inds
                           -- Just indices0 = Map.lookup (getTID t0) imap
                           -- ci = indices0 !! i0
                           -- -- for loop
                           
                           -- genCode' indent' target t0 imap
                           -- genCode' indent' target t1 imap
        
        -- $ target ++ " = 0.0;" $$
        

  -- in nest indent doc


