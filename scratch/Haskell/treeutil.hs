module TreeUtil(
  TOrder(..),
  TDirection(..),
  traverseS
) where

import Control.Monad.State
import Data.Tree


data TOrder = Pre | Post
data TDirection = LtoR | RtoL

type StateTrafo s a   = s -> a -> s
type DataTrafo  s a b = s -> a -> b


traverseS :: TOrder -> TDirection ->
                     (StateTrafo s a) -> (DataTrafo s a b) -> Tree a ->
                     State s (Tree b)

traverseS Pre LtoR strafo dtrafo t = do
  s <- get
  let a  = rootLabel t
      b  = dtrafo s a
      s' = strafo s a
  put s'
  subForest' <- foldM (traversalFolderS Pre LtoR strafo dtrafo)
                      []
                      (subForest t)
  return $ Node b subForest'
  
traverseS Pre RtoL strafo dtrafo t = do
  s <- get
  let a  = rootLabel t
      b  = dtrafo s a
      s' = strafo s a
  put s'
  subForest' <- foldM (traversalFolderS Pre RtoL strafo dtrafo)
                      []
                      (reverse . subForest $ t)
  return $ Node b (reverse subForest')
  
traverseS Post LtoR strafo dtrafo t = do
  subForest' <- foldM (traversalFolderS Post LtoR strafo dtrafo)
                      []
                      (subForest t)
  s <- get
  let a  = rootLabel t
      b  = dtrafo s a
      s' = strafo s a
  put s'
  return $ Node b subForest'
  
traverseS Post RtoL strafo dtrafo t = do
  subForest' <- foldM (traversalFolderS Post RtoL strafo dtrafo)
                      []
                      (reverse . subForest $ t)
  s <- get
  let a  = rootLabel t
      b  = dtrafo s a
      s' = strafo s a
  put s'
  return $ Node b (reverse subForest')

    
traversalFolderS :: TOrder -> TDirection ->
                    (StateTrafo s a) -> (DataTrafo s a b) ->
                    [Tree b] -> Tree a -> State s [Tree b]  
traversalFolderS ord dir strafo dtrafo bts at = do
  bt <- traverseS ord dir strafo dtrafo at
  return $ bts ++ [bt]
  


-- not currently used:
traverseWithoutState :: TOrder -> TDirection ->
                        (StateTrafo s a) -> (DataTrafo s a b) -> s ->
                        Tree a -> Tree b
                     
traverseWithoutState Pre LtoR strafo dtrafo s t =
  let a  = rootLabel t 
      b  = dtrafo s a
      s' = strafo s a
      subForest' = foldl (traverser Pre LtoR strafo dtrafo s')
                         []
                         (subForest t)
  in Node b subForest'

traverseWithoutState Pre RtoL strafo dtrafo s t =
  let a  = rootLabel t
      b  = dtrafo s a
      s' = strafo s a
      subForest' = foldl (traverser Pre LtoR strafo dtrafo s')
                         []
                         (reverse . subForest $ t)
  in Node b (reverse subForest')
  
traverseWithoutState Post LtoR strafo dtrafo s t = 
  let subForest' = foldl (traverser Pre LtoR strafo dtrafo s)
                         []
                         (subForest t)
      a  = rootLabel t
      s' = strafo s  a
      b  = dtrafo s' a
  in Node b subForest'

traverseWithoutState Post RtoL strafo dtrafo s t = 
  let subForest' = foldl (traverser Pre LtoR strafo dtrafo s)
                         []
                         (reverse . subForest $ t)
      a  = rootLabel t
      s' = strafo s  a
      b  = dtrafo s' a
  in Node b (reverse subForest')

  
traverser :: TOrder -> TDirection ->
             (StateTrafo s a) -> (DataTrafo s a b) -> s ->
             [Tree b] -> Tree a -> [Tree b]
traverser ord dir strafo dtrafo s bts at =
  let bt = traverseWithoutState ord dir strafo dtrafo s at
  in bts ++ [bt] 
