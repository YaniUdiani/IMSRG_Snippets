#ifndef IMSRG_ABODYOP_H
#define IMSRG_ABODYOP_H

#include "BodyOp.h"
#include "Basis/APB.h"
#include <vector>

class ABodyOp : public Debuggable_Obj{

public:
    static int numberOfExistingABodyOps;
    std::string name = "A-Body Operator: PLEASE NAME ME! :]";
    std::string hermiticity;
    bool antiSymmetrized, normalOrdered;

private:
    bool InputsVerified = false;

protected:
    APBasis *pBasis = nullptr;

private:
    BodyOp **pBodyOps = nullptr; // Pointer to array of pointers to maxRank + 1 Body operators


    //--------------------------------------- Generic Loop Functions ---------------------------------------------------
    // See: https://tinyurl.com/2p96vhxs and https://tinyurl.com/2hbcrs6t
    // This function expects a pointer to a void ABodyOp::Func(unsigned int)
    void LoopOverBodyOps(void (ABodyOp::*Func) (unsigned int));
    void LoopOverBodyOps(void (ABodyOp::*Func) (unsigned int, void *data), void *data);

    //----------------------------- Memory allocation and de-allocation functions --------------------------------------

    void MallocBodyOpArr();
    void DeallocBodyOps() const noexcept;

public:

    void CallocBodyOp(unsigned int rank, const std::string& rankName);
    void CallocBodyOps(const std::vector<unsigned int>& enableRanks, const std::vector<std::string>& names);
    void DeallocBodyOp(unsigned int rank);
    void DeallocBodyOps(const std::vector<unsigned int>& disableRanks);
    void DeallocBodyOpsAboveThisRank(unsigned cutoffRank);
    virtual ~ABodyOp();

    //------------------------------------- Constructors and Assignment ------------------------------------------------

    // First declare helper functions for determining names and ranks
    static std::vector<std::string> SetNames(const std::string &sharedName,const std::vector<unsigned int> &enableRanks,
                                             APBasis *basis);
    static std::vector<unsigned> SetRanks(APBasis *basis);

    ABodyOp() noexcept = delete;
    explicit ABodyOp(const std::vector<unsigned int> &enableRanks, const std::vector<std::string> &names,
                     const std::string& setHermiticity, bool isAntiSymmetrized, bool isNormalOrdered, APBasis *basis);
    explicit ABodyOp(const std::vector<unsigned int> &enableRanks, const std::string &sharedName,
                     const std::string &setHermiticity, bool isAntiSymmetrized, bool isNormalOrdered, APBasis *basis);
    explicit ABodyOp(const std::vector<unsigned int> &enableRanks, const std::string &sharedName,
                     const std::string &setHermiticity, APBasis *basis);
    explicit ABodyOp(const ABodyOp &usingThis, const std::string &plsNameMe, const std::string &setHermiticity);
    explicit ABodyOp(const ABodyOp &usingThis, const std::string &plsNameMe);
    // Enables all possible ranks in the basis
    explicit ABodyOp(const std::string &sharedName, const std::string &setHermiticity, APBasis *basis);

    // Defines how a newly created ABodyOp copies resources from another ABodyOp
    ABodyOp(const ABodyOp& other) noexcept;
    // Defines how a newly created ABodyOp steals resources from another ABodyOp
    ABodyOp(ABodyOp&& other) noexcept;
    // Defines how an existing ABodyOp copies resources from another ABodyOp
    ABodyOp& operator=(const ABodyOp& other) noexcept;
    // Defines how an existing ABodyOp steals resources from another ABodyOp
    ABodyOp& operator=(ABodyOp&& other) noexcept;

private:
    void NaiveCopyAssign(const ABodyOp &other) noexcept;
    void SmartCopyAssign(const ABodyOp &other) noexcept;

protected:

    //---------------------------------------- Tests and Error Messages ------------------------------------------------

    void VerifyInputs(const std::vector<unsigned int>& enableRanks, const std::vector<std::string>& names);
    void VerifyAddition(const ABodyOp& other) const;
    void VerifyCommutator(const ABodyOp& other) const;
    void DebugMSG(const std::string& msg) const noexcept;
    void NotificationMSG(const std::string& msg) const noexcept;
    void WarningMSG() const;
    void WarningMSG(const std::string& msg) const;
    void WarningMSG(const ABodyOp& other, const std::string& msg) const;
    void ErrorMSG() const;
    void ErrorMSG(const std::string& msg) const;
    void ErrorMSG(const ABodyOp& other) const;
    void Error(const std::string& msg) const;
    void RankError() const;
    void RankError(unsigned int rank, const std::string& msg = "") const;
    void CheckBounds(unsigned int rank) const;

    //----------------------------------------------- Print data -------------------------------------------------------

    void PrintActiveRank(unsigned int i) noexcept;

public:

    void Print(bool verbose = true) noexcept;
    void PrintBodyOp(unsigned int rank) const;
    void PrintBodyOp(unsigned int rank, unsigned int blockNumber) const;
    void PrintBlockDistribution() const noexcept;
    void PrintNorms(const std::string &label = "A") const noexcept;
    void Print2BodyNorms(const std::string &label = "A") const noexcept;
    void PrintFancyNorms(const std::string &label = "A") const noexcept;

    //--------------------------------------------- Operations  --------------------------------------------------------

    void ResetZero(unsigned int rank) const;
    // This won't yell if there's a nullptr since it's supposed to conveniently reset everything
    void ResetZero() const;
    template<class T>
    void Fill(const T &val){
        for(unsigned int i = 0U; i <= pBasis->pOpData->maxRank; i++){
            if(pBodyOps[i] != nullptr) pBodyOps[i]->Fill(val);
        }
    }
    void CWiseTimesEqual(const ABodyOp &rhs);
    void CWiseDivideEqual(const ABodyOp &rhs);
    template<class T>
    ABodyOp& operator*=(const T& rhs){
        for(unsigned int i = 0U; i <= pBasis->pOpData->maxRank; i++){
            if(pBodyOps[i] != nullptr) *pBodyOps[i] *= rhs;
        }
        return *this;
    }
    ABodyOp& operator+=(const ABodyOp& rhs);
    ABodyOp& operator-=(const ABodyOp& rhs);
    template<class T>
    const ABodyOp operator*(const T& other) const{
        ABodyOp result = *this;
        result *= other;
        result.name = "Scaled: " + name;
        return result; //todo: this is an rvalue right?
    }
    const ABodyOp operator+(const ABodyOp& other) const;
    const ABodyOp operator-(const ABodyOp& other) const;
    bool operator==(const ABodyOp& other) const;
    bool operator!=(const ABodyOp& other) const;

    template<const bool debugModeIsOn = debugModeOn>
    BodyOp& operator[](unsigned int rank){
        if constexpr(debugModeIsOn){ // Run checks during development
            CheckBounds(rank);
            if (pBodyOps[rank] == nullptr) RankError(rank);
        }
        return *pBodyOps[rank];
    }
    template<const bool debugModeIsOn = debugModeOn>
    const BodyOp& operator[](unsigned int rank) const{ // If *this is const, then *pBodyOps[rank] should be const as well
        if constexpr(debugModeIsOn){ // Run checks during development
            CheckBounds(rank);
            if (pBodyOps[rank] == nullptr) RankError(rank);
        }
        return *pBodyOps[rank];
    }
/*
    ABodyOp& CommutatorManager(const ABodyOp& other) const;
    ABodyOp& operator&(const ABodyOp& other) const;
*/

    //---------------------------------------------- Interface  --------------------------------------------------------

    double GetMemoryCostGB() const noexcept;
    bool ThisRankIsEnabled(unsigned rank) const noexcept;
    std::vector<unsigned int> GetRankStatus() const noexcept;
    std::vector<unsigned int> GetEnabledRanks() const noexcept;
    unsigned int GetMaxEnabledRank() const noexcept;
    unsigned int GetNumEnabledRanks() const noexcept;
    APBasis *GetAPB() const noexcept;
    void NormalOrder(unsigned cutoffRank = dummyUnsignedInt);
    std::vector<MatriX::Scalar> FrobeniusNorm() const noexcept;
    std::vector<MatriX::Scalar> OffDiagNorm() const;
    MatriX::Scalar OffDiagNorm(unsigned rank) const;
    std::vector<MatriX::Scalar> MinimumDiagNorm() const;
    MatriX::Scalar OffDiagNorm1BD() const;
    MatriX::Scalar OffDiagNorm2BD() const;
    MatriX::Scalar DiagNorm2BD() const;
    MatriX::Scalar OffDiagNorm3BD() const;
    MatriX::Scalar Norm3p1hAnd3h1p() const;
    void ResetZero3p1hAnd3h1pSector();
    void Fill3p1hAnd3h1pSector(const MatriX::Scalar &val);
    MatriX::Scalar AuxillaryDiagNorm2BD() const;
    MatriX::Scalar Norm4Holes() const;
    MatriX::Scalar Normhphp() const;
    MatriX::Scalar Norm4Particles() const;
    void VerifyHermiticity(double tolerance = 1e-12) const noexcept;
    void VerifyHermiticity(unsigned rank, double tolerance = 1e-12) const noexcept;
    double HermiticityDeviation(bool beVerbose = false) const noexcept;
    double HermiticityDeviation(unsigned rank) const noexcept;
    void PrintHermiticityDeviation(const std::string &label = "H") const noexcept;
    void Compress() noexcept;
    void Compress(unsigned rank);
    void Compress(const ABodyOp &usingThis) noexcept;
    void Compress(const ABodyOp &usingThis, unsigned rank);
    double CompressionError(const ABodyOp &usingThis, unsigned rank) const;
    double RankAverageCompressionError(const ABodyOp &usingThis) const;

    void AntiSymmetrize() noexcept;
    void AntiSymmetrize(unsigned rank);
    void Assign1stQuadrantUpperTriangle(unsigned rank);
    bool SameEnabledRanks(const ABodyOp &as) const;

protected:
    void AntiSymmetrize2BD();
    void AntiSymmetrize3BD();
    void NormalOrder1BD() noexcept;
    void NormalOrder2BD() noexcept;
    void NormalOrder3BD() noexcept;
};

#endif //IMSRG_ABODYOP_H
