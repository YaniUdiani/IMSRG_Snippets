/**
 * Commutators are comprised of many tensor contractions between BodyOps (components of ABodyOps). 
 * Commutators are the most performance limiting operations in the code, and are thus highly optimized.
 * Commute() is optimized with OpenMP, BLAS, cuBLAS, and MPI as well as symmetry exploitations and clever rewritings of tensor contractions
 */
void Commutator::Commute(const ABodyOp &op1, const ABodyOp &op2, ABodyOp &buffer){
    VerifyInputs(op1, op2, buffer);
    PrepareBuffer(op1, op2, buffer);
    const unsigned maxRank = pBasis->maxRank;
    for(unsigned i = 0U; i <= maxRank; i++){
        for(unsigned j = 0U; j <= maxRank; j++){
            if(op1.ThisRankIsEnabled(i) and op2.ThisRankIsEnabled(j)){
                Commute(op1[i], op2[j], buffer); // Trade secrets are here, not disclosed :^) But note: optimizations are made in here
            }
        }
    }
}
