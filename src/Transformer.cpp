/**
 * Transformers inherit from ABodyOps. Transformer::Transform() is used to perform the various series 
 * expansions in the Magnus expansion formulation of the IMSRG. It uses the various algebraic operations 
 * defined for ABodyOps--making the code clear to understand. It also uses two buffers to cleverly 
 * cache recursive commutators (tensor contractions).
 */
void Transformer::Transform(const ABodyOp &operand, ABodyOp &buffer, const std::vector<double> &seriesCoeffs,
                            const std::vector<double> &coeffsForConvergenceCheck, const unsigned rankForConvergenceCheck,
                            const double tolerance, const double seriesCoeffTolerance){

    if(temps->GetNumTemps() < 2U){
        Error(std::string("Failure in Transformer::Transform(): temps only contains ")
              + std::to_string(temps->GetNumTemps())
              + std::string(" ABodyOps. A minimum of 2 is needed to store results from the nested commutators.")
              );
    }
    if(seriesCoeffs.empty()){
        Error("Failure in Transformer::Transform(): Input series has no coefficients.");
    }
    if(seriesCoeffs.size() != coeffsForConvergenceCheck.size()){
        Error("Failure in Transformer::Transform(): seriesCoeffs.size != coeffsForConvergenceCheck.size().");
    }
    if(&operand == &buffer and seriesCoeffs[0U] != 1.0){
        Error("Failure in Transformer::Transform(): &operand == &buffer, yet seriesCoeffs[0U] != 1.0.");
    }

    CollectionOfTemps &arrayOfTemps = *temps; // We'll treat temps like an array!
    const std::vector<double> &cvgCoeff = coeffsForConvergenceCheck;
    const unsigned maxOrder = seriesCoeffs.size()-1U;
    const unsigned rank = rankForConvergenceCheck;

    // Set C = c_0 * B
    buffer = operand;
    buffer *= seriesCoeffs[0U];

    // Run checks to see if we need to go past 0th order
    if(maxOrder == 0U) {return;}
    if(maxOrder == 1U and abs(seriesCoeffs[1U]) <= seriesCoeffTolerance) {return;}

    // Now, compute 1st order adjoint = [A,B]
    ABodyOp &adjoint = arrayOfTemps[0U];
    commutator->Commute(*this, operand, adjoint);

    if(maxOrder == 1U){
        adjoint *= seriesCoeffs[1U];
        buffer += adjoint;
        return;
    }

    // Now, sum remaining terms into C
    for(unsigned n = 2U; n <= maxOrder; n++){
        ABodyOp &previousAdjoint = arrayOfTemps[n % 2U]; // Stores [A, B]^(n-1)
        ABodyOp &currentAdjoint = arrayOfTemps[(n + 1U) % 2U]; // Available to store [A, B]^(n)

        // Compute [A, [A, B]^(n-1)] if [A, B]^(n-1) != "0"
        const auto prevNorm = abs(cvgCoeff[n-1U] * previousAdjoint[rank].FrobeniusNorm());
        if(prevNorm > tolerance){
            commutator->Commute(*this, previousAdjoint, currentAdjoint);
        }
        // Sum: C += c_{n-1} * [A, B]^(n-1) if c_{n-1} != "0"
        if(abs(seriesCoeffs[n-1U]) > seriesCoeffTolerance){
            previousAdjoint *= seriesCoeffs[n-1U];
            buffer += previousAdjoint;
        }
        // If [A, B]^(n-1) = "0", then the series has converged
        if(prevNorm <= tolerance) {
            break;
        }

        // If the series hasn't yet converged, but we're at n = maxOrder, sum: C += c_{maxOrder} * [A, B]^(maxOrder)
        if(n == maxOrder){
            if(abs(seriesCoeffs[maxOrder]) > seriesCoeffTolerance){
                currentAdjoint *= seriesCoeffs[maxOrder];
                buffer += currentAdjoint;
            }
        }
    }
}
