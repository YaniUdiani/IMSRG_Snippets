# Code Snippets from my Graduate Work on IMSRG for Nuclear Matter
Neutron stars are of great interest in physics, as understanding them sheds light on the existence of new possible
states of matter, and also on the origin of heavy elements within our solar system. For my doctorate, I applied the quantum many-body method
In-Medium Similarity Renormalization Group (IMSRG) to study neutron stars by solving coupled ODEs (“flow equations”) 
that govern the evolution of nuclear interactions on a finite grid of momenta with periodic boundary conditions. 
As the chief program architect in a team of four scientists, I built a state-of-the-art high performance program from
the ground up that simulates many interacting nucleons within a neutron star to output physical quantities 
(eigenvalues) that determine its pressure, stability, and 3D properties.

> **Our Nuclear Matter IMSRG program is proprietary to the Facility for Rare Isotope Beams (FRIB) and was decided to be kept private.
This repository displays snippets of the code to the general public. See the description of each file below. Also see the **
### ABodyOp.h
ABodyOps are a foundational data structure of our IMSRG code. They store pointers to BodyOps, which store blocks of Eigen matrices. 
In our largest calculations, ABodyOps allocate a total of ~1 TB of RAM via BodyOps. Proper memory management of ABodyOps 
and their subsidiaries BodyOps is therefore crucial. Moreover, commutators (tensor contractions) between them are the most performance 
limiting operations in the code. To achieve the code's performance at scale, I made a multitude of optimizations to ABodyOps and their commutators.
