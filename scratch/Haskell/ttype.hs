module TType (
  TIdx, TInt,
  TType,
  rank, dim, size, isScalarTType,
  makeTType,
  makeScalarTType,
  makeProductTType,
  makeContractionTType,
  makeTransposedTType
  ) where

-- Type for indexing tensors and tensor types:
type TIdx = Int

-- 'TType' represents the type of a tensor, i.e. a list of integer dimensions:
type TInt = Int
type TType = [TInt]

makeTType :: [TInt] -> TType
makeTType = id

rank :: TType -> TInt
rank ty = length ty

dim :: TType -> TIdx -> TInt
dim ty i = ty !! i

size :: TType -> TInt
size = product

scalarTType = []

isScalarTType :: TType -> Bool
isScalarTType ty = ty == scalarTType

makeScalarTType :: TType
makeScalarTType = scalarTType

makeProductTType :: TType -> TType -> TType
makeProductTType t0 t1 = t0 ++ t1

makeContractionTType :: TType -> TIdx -> TType -> TIdx -> TType
makeContractionTType t0 i0 t1 i1 =
  let t0' = take i0 t0 ++ drop (i0+1) t0
      t1' = take i1 t1 ++ drop (i1+1) t1
  in makeProductTType t0' t1'

makeTransposedTType :: TType -> TIdx -> TIdx -> TType
makeTransposedTType t i0 i1
  | i0 == i1 = t
  | otherwise =
      let i0' = min i0 i1
          i1' = max i0 i1
          d0 = dim t i0'
          d1 = dim t i1'
          head = take i0' t
          tail = drop (i0'+1) t
          middle = take (i1'-i0'-1) tail
          end = drop (i1'-i0') tail
      in head ++ [d1] ++ middle ++ [d0] ++ end 



