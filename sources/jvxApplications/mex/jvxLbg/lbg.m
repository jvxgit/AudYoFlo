% LBG  codebook design for vector quantizer (MEX version)
%                                                                            
%    LBG creates a new codebook or retrains an existing codebook              
%    using training data as reference                                        
%    LBG is based on the Linde-Buzo-Gray training algorithm                     
%    the new codebook can then be used for a vector-quantization             
%
% usage   :                                                                   
% [newcodebook,abserr,relerr,it]                                              
%             =lbg(traindat,oldcodebook,ny,energy-limit,energy-diff-limit)    
%                                                                             
%    newcodebook   : centroids resulting from lbg algorithm                   
%                    the dimension of the newcodebook vectors is             
%                    the same as the dimension of traindat/oldcodebook        
%    abserr        : (optional) error energy at algorithm breakoff             
%    relerr        : (optional) difference between last two error energies    
%                    before breakoff                                          
%    it            : (optional) number of iteration steps                                
%    traindat      : data which is used to train the codebook                 
%    oldcodebook   : existing codebook which is going to be adapted           
%                    if no old codebook exists, enter []                      
%    ny            : (optional) number of vectors in newcodebook              
%                    must be equal to number of vectors in oldcodebook        
%                    is no more optional when oldcodebook does not exist      
%    energy-limit  : (optional) error energy which should lead to breakoff    
%                    default value = 0                                        
%    energy-diff-limit : (optional) difference between                        
%                        two error energies which should lead to breakoff     
%                        default value = 1e-06                                
%    NOTE:                                                                    
%    All vectors are understood to be COLUMN VECTORS                           
%    In a matrix the columns are considered as vectors                        
%       the number of columns rerturns the number of vectors                  
%       the number of rows returns the dimension of the vectors               
%                                                                             
%    An optional input is still necessary if the                             
%    optional variable to its right is given!   
