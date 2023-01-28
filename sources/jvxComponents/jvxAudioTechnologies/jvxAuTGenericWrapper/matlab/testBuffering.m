function testBuffering(bSize, bSizeUsed, lowComplexity, durationProc)
%bSize = 7;
%bSizeUsed = 10;
fSizeMax = 0;
szBuffer = 0;


periodHW = ceil(bSize/(bSizeUsed-bSize));
periodSW = bSizeUsed/gcd(bSize, bSizeUsed);

%numSamplesHWIn = 0;
%numSamplesHWOut = (1+ceil(bSizeUsed/bSize))*bSize;

if(lowComplexity)
    dBuf = [];
    numSamplesSimu = 2*bSize*bSizeUsed;
    processingActive = zeros(1, numSamplesSimu+2);
    numSamplesInEnd = zeros(1, numSamplesSimu+2);
    numSamplesOutEnd = zeros(1, numSamplesSimu+2);
    
    processingActive(1) = -4;
    sizeBufferHWIn = bSize+bSizeUsed;
    sizeBufferHWOut = bSize+2*bSizeUsed;
    
    %durationProc = bSizeUsed;%bSizeUsed-2+ round((rand(1)-0.5)*2*3);
    alpha = 1.0;
    if(mod(bSizeUsed, bSize) == 0)
        numSamplesHWIn = bSizeUsed;
        numSamplesHWOut = round(bSizeUsed*alpha);
    else
        numSamplesHWIn = bSizeUsed;
        numSamplesHWOut = bSize + round(alpha * bSizeUsed);
    end
        
    
    numSamplesInEnd(1) = numSamplesHWIn;
    numSamplesOutEnd(1) = numSamplesHWOut;

    time = -1;
    % numSamplesHWIn = (bSizeUsed-bSize)*periodHW;
    % numSamplesHWOut = bSize*ceil(bSizeUsed/bSize);
    
    bufferToBeProcessed = false;

    disp(['NumberSamples InputBuffer:' num2str(numSamplesHWIn)]);
    disp(['NumberSamples OutputBuffer:' num2str(numSamplesHWOut)]);
    
    cntProcessingSW = -1;
    for(tIdx = 0:numSamplesSimu)
        %disp(['Time: ' num2str(tIdx)]);
        %disp(['Processing Counter: ' num2str(cntProcessingSW)]);
        
        if(cntProcessingSW > 0)
            processingActive(tIdx+2) = -2;
        else
            processingActive(tIdx+2) = -4;
        end
        anyChange = false;
        
        if(cntProcessingSW == 0)
            disp('----> Software Interrupt');
            anyChange = true;
            disp([num2str(bSizeUsed) ' new output samples produced.']);
            numSamplesHWOut = numSamplesHWOut + bSizeUsed;
            cntProcessingSW = -1;
            if(numSamplesHWIn >= bSizeUsed)
                disp(['Start processing of ' num2str(bSizeUsed) ' samples.']);
                cntProcessingSW = durationProc;
                dBuf = [dBuf durationProc];
                numSamplesHWIn = numSamplesHWIn-bSizeUsed;
            end
        end
        
        if(mod(tIdx, bSize) == 0)
            disp('::::::> Hardware Interrupt');
            disp([num2str(bSize) ' samples HW In and Out.']);
            
            anyChange = true;
            % New Hardware Frame
            %if(numSamplesHWOut > bSize)
                numSamplesHWIn = numSamplesHWIn + bSize;
                numSamplesHWOut = numSamplesHWOut - bSize;
           
                if(numSamplesHWIn >= bSizeUsed)
                    if(cntProcessingSW < 0)
                        disp(['Start processing of ' num2str(bSizeUsed) ' samples.']);
                        dBuf = [dBuf durationProc];
                        cntProcessingSW = durationProc;
                        numSamplesHWIn = numSamplesHWIn-bSizeUsed;
                    end
                end
            %end
        end
        
        if(cntProcessingSW > 0)
            cntProcessingSW = cntProcessingSW-1;
        end
        
        numSamplesInEnd(tIdx+2) = numSamplesHWIn;
        numSamplesOutEnd(tIdx+2) = numSamplesHWOut;

        
        
        if(anyChange)
            disp(['NumberSamples InputBuffer:' num2str(numSamplesHWIn)]);
            disp(['NumberSamples OutputBuffer:' num2str(numSamplesHWOut)]);
            %input('Weiter');
        end
    end
    time = [-1 [0:numSamplesSimu]];
    
    figure; hold on; plot(time, numSamplesInEnd);plot(time, sizeBufferHWIn*ones(1, length(time)), 'm');...
        plot(time, numSamplesOutEnd, 'r');plot(time, sizeBufferHWOut*ones(1, length(time)), 'g');...
        plot(time, processingActive, 'k:');legend({'Input', 'Input', 'Output', 'Output'});
    dBuf
else
    sizeBufferHWIn = bSize+bSizeUsed;
    sizeBufferHWOut = bSize+bSizeUsed;
    periodHW = bSizeUsed/gcd(bSize, bSizeUsed);
    
    procID = [];
    numSamplesHWIn = bSizeUsed;
    numSamplesHWOut = 0;%bSize + 6;
    bufferToBeProcessed = false;

    disp(['NumberSamples InputBuffer:' num2str(numSamplesHWIn)]);
    disp(['NumberSamples OutputBuffer:' num2str(numSamplesHWOut)]);
    
    cntProcessingSW = -1;

    cntSwitches = 0;
    
    numSamplesInStart = numSamplesHWIn;
    numSamplesOutStart = numSamplesHWOut;

    numSamplesInEnd = numSamplesHWIn;
    numSamplesOutEnd = numSamplesHWOut;
    tIdx = 0;
    while(1)        

        if(mod(tIdx, bSize) == 0)
            disp('::::::> Hardware Interrupt');
            disp(['BB: NumberSamples InputBuffer:' num2str(numSamplesHWIn)]);
            disp(['BB: NumberSamples OutputBuffer:' num2str(numSamplesHWOut)]);
            
            anyChange = true;
            % New Hardware Frame
            numSamplesHWIn = numSamplesHWIn + bSize;
            
            disp(['Add ' num2str(bSize) ' input samples']);
            disp(['NumberSamples InputBuffer:' num2str(numSamplesHWIn)]);
            disp(['NumberSamples OutputBuffer:' num2str(numSamplesHWOut)]);
            
            numSamplesInStart = [numSamplesInStart numSamplesHWIn];
            numSamplesOutStart = [numSamplesOutStart numSamplesHWOut];

            if(numSamplesHWIn >= bSizeUsed)
                cL = 0;
                while(numSamplesHWIn >= bSizeUsed)
                    disp(['==== Start processing of ' num2str(bSizeUsed) ' samples.']);
                
                    numSamplesHWIn = numSamplesHWIn-bSizeUsed;
                    numSamplesHWOut = numSamplesHWOut+bSizeUsed;
                    cL = cL +1;
                    disp(['==== Processing finshed,  produced ' num2str(bSizeUsed) ' output samples.']);
                    disp(['NumberSamples InputBuffer:' num2str(numSamplesHWIn)]);
                    disp(['NumberSamples OutputBuffer:' num2str(numSamplesHWOut)]);
                end
                procID = [procID cL];
            else
                procID = [procID 0];
            end
            
            disp(['Write ' num2str(bSize) ' output samples']);
            numSamplesHWOut = numSamplesHWOut - bSize;
            disp(['NumberSamples InputBuffer:' num2str(numSamplesHWIn)]);
            disp(['NumberSamples OutputBuffer:' num2str(numSamplesHWOut)]);
            
            numSamplesInEnd = [numSamplesInEnd numSamplesHWIn];
            numSamplesOutEnd = [numSamplesOutEnd numSamplesHWOut];

            if(numSamplesHWOut < 0)
                disp(['Error']);
                break;
            end
            
            cntSwitches = cntSwitches +1;
            if(cntSwitches == periodHW)
                sizeBufferHWIn
                numSamplesInStart
                sizeBufferHWOut
                numSamplesOutStart
                numSamplesInEnd
                numSamplesOutEnd
                procID
                disp('End of loop');
                break;
            end
            %            if(numSamplesHWIn > sizeBufferHWIn)
            %                disp('Error #1');
            %            end
            %            if(numSamplesHWOut < 0)
            %                disp('Error #2');
            %            end
               
           input('Weiter');
        end
        %disp(['Time: ' num2str(tIdx)]);
        %disp(['Processing Counter: ' num2str(cntProcessingSW)]);
        
        tIdx = tIdx +1;
    end
end
