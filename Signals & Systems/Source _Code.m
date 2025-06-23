clear; clc; close all;
set(0,'defaultfigurecolor','w') ;

% Declaration of Parameters
BASE_FOLDER_DRONE = 'C:\Users\jiebro\Desktop\ZJU\Second Year Latter\Signals & Systems\Design\Original_Data\Exist_UAV';
BASE_FOLDER_NO_DRONE = 'C:\Users\jiebro\Desktop\ZJU\Second Year Latter\Signals & Systems\Design\Original_Data\None_UAV';
FS = 5120;                        % Sampling frequency in Hz 
FRAME_LENGTH_SAMPLES = 256;       % Frame length 
FRAME_HOP_SAMPLES = 128;          % step size
NUM_MFCC_COEFFS = 12;             % Number of MFCC coefficients
NUM_MEL_FILTERS = 24;             % Number of Mel filters 
PRE_EMPH_COEFF = 0.9375;          % Pre-emphasis coefficient 
TRAIN_RATIO = 0.75;               % Percentage of data in the test set

allAudioData = {}; % Save all data
allLabels = []; % 1 for drone, 0 for no-drone

% Load Drone Sounds
droneFiles = dir(fullfile(BASE_FOLDER_DRONE, '*.mat')); % Open the folder
for i = 1:length(droneFiles)
    filePath = fullfile(BASE_FOLDER_DRONE, droneFiles(i).name);
    matData = load(filePath);
    % Assuming the audio data is in a variable named 'data' or the first variable
    fnames = fieldnames(matData);
    audioSignal = matData.(fnames{1});
    allAudioData{end+1} = audioSignal;
    allLabels(end+1) = 1;
end

% Load No-Drone Sounds
noDroneFiles = dir(fullfile(BASE_FOLDER_NO_DRONE, '*.mat'));
for i = 1:length(noDroneFiles)
    filePath = fullfile(BASE_FOLDER_NO_DRONE, noDroneFiles(i).name);
    matData = load(filePath);
    fnames = fieldnames(matData);
    audioSignal = matData.(fnames{1});
    allAudioData{end+1} = audioSignal;
    allLabels(end+1) = 0;
end

% Feature Extraction
allFeatures = [];
allFrameLabels = [];

% Define Hamming window for enframe or manual application
window = hamming(FRAME_LENGTH_SAMPLES); % 

% DCT matrix
dctcoef = zeros(NUM_MFCC_COEFFS, NUM_MEL_FILTERS);
for k = 1:NUM_MFCC_COEFFS
    n_dct = 0:(NUM_MEL_FILTERS-1);
    dctcoef(k,:) = cos((2*n_dct+1)*k*pi/(2*NUM_MEL_FILTERS));
end

% Cepstral liftering weights
N_lifter = 1:NUM_MFCC_COEFFS;
lifter_w = 1 + 6 * sin(pi * N_lifter' / NUM_MFCC_COEFFS); 
lifter_w = lifter_w / max(lifter_w); % Normalize 

% Mel filter bank 
melBank = melbankm(NUM_MEL_FILTERS, FRAME_LENGTH_SAMPLES, FS, 0, 0.5, 'm'); % 'm' for Mel scale
melBank = full(melBank);
melBank = melBank / max(melBank(:)); % Normalize 

% Plotting setup for one drone and one no-drone file
droneFileIdx = find(allLabels == 1, 1, 'first');
noDroneFileIdx = find(allLabels == 0, 1, 'first');
plottedWindowEffect_withdrone = false;
plottedWindowEffect_nodrone = false;
plottedSpectrograms = false;

% Plotting setup for heat map
mfccs_drone_plot_static = [];
mfccs_nodrone_plot_static = [];
mfccs_drone_plot_delta = [];
mfccs_nodrone_plot_delta = [];

for i = 1:length(allAudioData)
    signal = allAudioData{i};
    currentLabel = allLabels(i);
    % Pre-emphasis 
    signal_preemph = filter([1 -PRE_EMPH_COEFF], 1, signal);

    % Framing 
    frames_raw = enframe(signal_preemph, FRAME_LENGTH_SAMPLES, FRAME_HOP_SAMPLES);
    frames=frames_raw;
    numFrames = size(frames, 1);
    mfccs_static_current_file = zeros(numFrames, NUM_MFCC_COEFFS);

    for j = 1:numFrames
        % Windowing
        frames(j, : )=frames(j, : )'.*window;
        frameCur = frames(j, :);

        % Show windowing effect
        if ~plottedWindowEffect_withdrone && currentLabel
            figure;
            subplot(2,2,1); plot(frames_raw(j,:)); title('Original Frame (after pre-emphasis)');
            subplot(2,2,2); plot(frameCur); title('Windowed Frame (Hamming)');
            subplot(2,2,3); plot(abs(fft(frames_raw(j, : ))).^2); title('Power Spectrum (Original Segment Approx.)');
            subplot(2,2,4); plot(abs(fft(frameCur)).^2); title('Power Spectrum (Windowed Frame)');
            sgtitle('Windowing Effect (With drone)');
            plottedWindowEffect_withdrone = true;
        end

        if ~plottedWindowEffect_nodrone && ~currentLabel
            figure;
            subplot(2,2,1); plot(frames_raw(j,:)); title('Original Frame (after pre-emphasis)');
            subplot(2,2,2); plot(frameCur); title('Windowed Frame (Hamming)');
            subplot(2,2,3); plot(abs(fft(frames_raw(j, : ))).^2); title('Power Spectrum (Original Segment Approx.)');
            subplot(2,2,4); plot(abs(fft(frameCur)).^2); title('Power Spectrum (Windowed Frame)');
            sgtitle('Windowing Effect (No drone)');
            plottedWindowEffect_nodrone = true;
        end

        % FFT
        frameFFT = fft(frameCur, FRAME_LENGTH_SAMPLES);
        % Power Spectrum 
        powerSpec = abs(frameFFT(1:(FRAME_LENGTH_SAMPLES/2 + 1))).^2;
        % Mel Filterbank Application 
        melPowerCur = melBank * powerSpec'; 
        melPowerCur_log = log(max(melPowerCur, 1e-6)); % Add small constant to avoid log(0)
        % DCT
        mfcc_frame_raw = dctcoef * melPowerCur_log;
        % Cepstral liftering 
        mfcc_frame_liftered = mfcc_frame_raw .* lifter_w; 
        mfccs_static_current_file(j, :) = mfcc_frame_liftered';
    end

    % Delta MFCCs
    mfccs_delta_current_file = zeros(numFrames - 4, NUM_MFCC_COEFFS);
    denominator_delta = 3;
    for k = 1:NUM_MFCC_COEFFS 
        m_coeff_col = mfccs_static_current_file(:, k);
        for frame_idx = 3:(numFrames - 2) 
            mfccs_delta_current_file(frame_idx-2, k) = ...
                (2*m_coeff_col(frame_idx+2) + m_coeff_col(frame_idx+1) - ...
                m_coeff_col(frame_idx-1) - 2*m_coeff_col(frame_idx-2)) / denominator_delta;
        end
    end

    % Storing the data needed to create a heat map
    if i == droneFileIdx
        mfccs_drone_plot_static = mfccs_static_current_file'; 
        mfccs_drone_plot_delta = mfccs_delta_current_file'; 
    end
    if i == noDroneFileIdx
        mfccs_nodrone_plot_static = mfccs_static_current_file'; 
        mfccs_nodrone_plot_delta = mfccs_delta_current_file'; 
    end

    % Combine static and delta features 
    mfccs_static_for_combination = mfccs_static_current_file(3:(numFrames-2), :);
    mfccs_combined = [mfccs_static_for_combination, mfccs_delta_current_file];
    valid_indices = 3:(numFrames-2);

    % Plot spectrogram and average MFCC for  with-drone/no-drone file
    if ~plottedSpectrograms && (i == droneFileIdx || i == noDroneFileIdx)
        figure;
        spectrogram(signal, window, FRAME_LENGTH_SAMPLES - FRAME_HOP_SAMPLES, FRAME_LENGTH_SAMPLES, FS, 'yaxis');
        if currentLabel == 1
            title(['Spectrogram of Drone Sound (File ', num2str(i), ')']);
        else
            title(['Spectrogram of No-Drone Sound (File ', num2str(i), ')']);
        end
    end

    allFeatures = [allFeatures; mfccs_combined];
    allFrameLabels = [allFrameLabels; repmat(currentLabel, size(mfccs_combined, 1), 1)];
end

disp(['Total frames for training/testing: ', num2str(size(allFeatures, 1))]);

% Data set segmentation
numTotalFrames = size(allFeatures, 1);
rng('default'); 
shuffledIndices = randperm(numTotalFrames); % 

numTrainFrames = floor(TRAIN_RATIO * numTotalFrames);
trainIndices = shuffledIndices(1:numTrainFrames);
testIndices = shuffledIndices(numTrainFrames+1:end);

X_train = allFeatures(trainIndices, :);
Y_train = allFrameLabels(trainIndices, :);
X_test = allFeatures(testIndices, :);
Y_test = allFrameLabels(testIndices, :);

disp(['Training samples: ', num2str(size(X_train, 1))]);
disp(['Testing samples: ', num2str(size(X_test, 1))]);

% Training
SVMModel = fitcsvm(X_train, Y_train, ...
    'Standardize', true, ...
    'KernelFunction', 'RBF', ...
    'KernelScale', 'auto');
disp('SVM training complete.');

% Prediction and Evaluation
disp('5. Predicting on test set and evaluating...');
Y_pred = predict(SVMModel, X_test);  

% Get the Confusion-Matrix
C = confusionmat(Y_test, Y_pred);
TN = C(1,1); % True Negative
FP = C(1,2); % False Positive
FN = C(2,1); % False Negative
TP = C(2,2); % True Positive

disp(['True Negatives (TN): ', num2str(TN)]);
disp(['False Positives (FP): ', num2str(FP)]);
disp(['False Negatives (FN): ', num2str(FN)]);
disp(['True Positives (TP): ', num2str(TP)]);

% Show the accuracy
correctPredictions = sum(Y_pred == Y_test);
totalTestSamples = length(Y_test);
accuracy = correctPredictions / totalTestSamples;
disp(['Accuracy: ', num2str(accuracy * 100), '%']);

% Heat mapping of static MFCC coefficients
figure('Name', 'Static MFCC Side-by-Side Comparison', 'NumberTitle', 'off', 'Position', [100, 100, 1000, 500]); 

min_static_val = min([mfccs_drone_plot_static(:); mfccs_nodrone_plot_static(:)]);
max_static_val = max([mfccs_drone_plot_static(:); mfccs_nodrone_plot_static(:)]);

% With drone
subplot(1, 2, 1);
imagesc(mfccs_drone_plot_static);
colormap('jet');
caxis([min_static_val, max_static_val]);
colorbar;
title('Static MFCCs (With Drone Sound)');
xlabel('Frame Number');
ylabel('MFCC Coefficient Index');
yticks(1:NUM_MFCC_COEFFS);
yticklabels(arrayfun(@(x) sprintf('C%d', x-1), 1:NUM_MFCC_COEFFS, 'UniformOutput', false));

% No drone
subplot(1, 2, 2);
imagesc(mfccs_nodrone_plot_static);
colormap('jet');
caxis([min_static_val, max_static_val]);
colorbar;
title('Static MFCCs (No Drone Sound)');
xlabel('Frame Number');
ylabel('MFCC Coefficient Index');
yticks(1:NUM_MFCC_COEFFS);
yticklabels(arrayfun(@(x) sprintf('C%d', x-1), 1:NUM_MFCC_COEFFS, 'UniformOutput', false));

sgtitle('Comparison of Static MFCCs for Drone vs. No-Drone Sound');

% Heat mapping of static MFCC coefficients
figure('Name', 'Delta MFCC Side-by-Side Comparison', 'NumberTitle', 'off', 'Position', [100, 100, 1000, 500]); % 新图窗

min_delta_val = min([mfccs_drone_plot_delta(:); mfccs_nodrone_plot_delta(:)]);
max_delta_val = max([mfccs_drone_plot_delta(:); mfccs_nodrone_plot_delta(:)]);

% With drone
subplot(1, 2, 1);
imagesc(mfccs_drone_plot_delta);
colormap('jet'); 
caxis([min_delta_val, max_delta_val]);
colorbar;
title('Delta MFCCs (With Drone Sound)');
xlabel('Frame Number');
ylabel('Delta MFCC Coefficient Index');
yticks(1:NUM_MFCC_COEFFS);
yticklabels(arrayfun(@(x) sprintf('dC%d', x-1), 1:NUM_MFCC_COEFFS, 'UniformOutput', false)); % 标记为 dC0, dC1...

% No drone
subplot(1, 2, 2);
imagesc(mfccs_nodrone_plot_delta);
colormap('jet'); 
caxis([min_delta_val, max_delta_val]);
colorbar;
title('Delta MFCCs (No Drone Sound)');
xlabel('Frame Number');
ylabel('Delta MFCC Coefficient Index');
yticks(1:NUM_MFCC_COEFFS);
yticklabels(arrayfun(@(x) sprintf('dC%d', x-1), 1:NUM_MFCC_COEFFS, 'UniformOutput', false)); 

sgtitle('Comparison of Delta MFCCs for Drone vs. No-Drone Sound');
