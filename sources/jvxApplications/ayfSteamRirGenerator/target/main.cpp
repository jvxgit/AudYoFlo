#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

#include <phonon.h>
#include <vector>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <algorithm>

// ---------- helpers ----------
static void check(IPLerror err, const char* what)
{
    if (err != IPL_STATUS_SUCCESS) {
        std::printf("Steam Audio error %d at %s\n", (int)err, what);
        std::exit(1);
    }
}

static void buildShoebox(float Lx, float Ly, float Lz,
    std::vector<IPLVector3>& V,
    std::vector<IPLTriangle>& T)
{
    V = {
        {0,0,0}, {Lx,0,0}, {Lx,Ly,0}, {0,Ly,0},
        {0,0,Lz}, {Lx,0,Lz}, {Lx,Ly,Lz}, {0,Ly,Lz}
    };

    // inward normals
    T.push_back({ 0,1,2 }); T.push_back({ 0,2,3 }); // floor (+Z)
    T.push_back({ 4,6,5 }); T.push_back({ 4,7,6 }); // ceiling (-Z)
    T.push_back({ 0,5,1 }); T.push_back({ 0,4,5 }); // y=0 (+Y)
    T.push_back({ 3,2,6 }); T.push_back({ 3,6,7 }); // y=Ly (-Y)
    T.push_back({ 0,3,7 }); T.push_back({ 0,7,4 }); // x=0 (+X)
    T.push_back({ 1,6,2 }); T.push_back({ 1,5,6 }); // x=Lx (-X)
}

// Direct sound: delay + 1/r gain
static void addDirectWhole(std::vector<float>& out,
    const std::vector<float>& in,
    int fs,
    IPLVector3 src,
    IPLVector3 mic,
    float c = 343.0f)
{
    IPLVector3 d{ src.x - mic.x, src.y - mic.y, src.z - mic.z };
    float dist = std::sqrt(d.x * d.x + d.y * d.y + d.z * d.z);
    int delay = (int)std::round((dist / c) * fs);
    float gain = 1.0f / std::max(dist, 0.1f);

    for (size_t n = 0; n < in.size(); ++n) {
        size_t k = n + (size_t)delay;
        if (k < out.size())
            out[k] += gain * in[n];
    }
}

static void IPLCALL log_callback(IPLLogLevel level, const char* message)
{
    const char* label = "INFO";
    switch (level)
    {
    case IPL_LOGLEVEL_WARNING:
        label = "WARN";
        break;
    case IPL_LOGLEVEL_ERROR:
        label = "ERROR";
        break;
    case IPL_LOGLEVEL_DEBUG:
        label = "DEBUG";
        break;
    default:
        label = "INFO";
        break;
    }

    fprintf(stderr, "[SteamAudio][%s] %s", label, message);
}

int main()
{
    const char* inPath = "O:\\audyoflo\\audio-win64\\release\\runtime\\bin\\input.wav";
    const char* outPath = "O:\\audyoflo\\audio-win64\\release\\runtime\\bin\\output.wav";

    // ---- read input wav (mono float32)
    unsigned int inCh = 0, inFs = 0;
    drwav_uint64 inFrames = 0;
    float* inData = drwav_open_file_and_read_pcm_frames_f32(inPath, &inCh, &inFs, &inFrames, nullptr);
    if (!inData) { std::printf("Failed to read %s\n", inPath); return 1; }
    if (inCh != 1) { std::printf("Please provide mono input.wav\n"); return 1; }

    const int fs = (int)inFs;
    const int blockSize = 1024;
    const float irSeconds = 2.0f;

    // ---- Steam Audio context
    IPLContextSettings cs{};
    cs.version = STEAMAUDIO_VERSION;
    cs.logCallback = log_callback;
    cs.flags = IPL_CONTEXTFLAGS_VALIDATION;
    IPLContext ctx = nullptr;
    check(iplContextCreate(&cs, &ctx), "iplContextCreate");

    // ---- scene + shoebox mesh
    IPLSceneSettings ss{};
    ss.type = IPL_SCENETYPE_DEFAULT;
    IPLScene scene = nullptr;
    check(iplSceneCreate(ctx, &ss, &scene), "iplSceneCreate");

    float Lx = 6.f, Ly = 4.f, Lz = 3.f;
    std::vector<IPLVector3> V;
    std::vector<IPLTriangle> T;
    buildShoebox(Lx, Ly, Lz, V, T);

    // simple material (low absorption)
    IPLMaterial mats[1] = { { {0.10f, 0.10f, 0.10f}, 0.2f, {0,0,0} } };
    std::vector<IPLint32> matIdx(T.size(), 0);

    IPLStaticMeshSettings ms{};
    ms.numVertices = (IPLint32)V.size();
    ms.numTriangles = (IPLint32)T.size();
    ms.numMaterials = 1;
    ms.vertices = V.data();
    ms.triangles = T.data();
    ms.materialIndices = matIdx.data();
    ms.materials = mats;

    IPLStaticMesh mesh = nullptr;
    check(iplStaticMeshCreate(scene, &ms, &mesh), "iplStaticMeshCreate");
    iplStaticMeshAdd(mesh, scene);
    iplSceneCommit(scene);

    

    
    // ---- simulator
    IPLSimulationSettings simS{};

    // Audio block config
    simS.samplingRate = fs;
    simS.frameSize = blockSize;

    simS.flags = (IPLSimulationFlags)(IPL_SIMULATIONFLAGS_REFLECTIONS); // (optional DIRECT too)
    simS.sceneType = ss.type;

    // IMPORTANT: In 4.8 these fields exist in IPLSimulationSettings:
    // Adjust names if needed (check phonon.h):
    simS.samplingRate = fs;     // <-- if your header uses different name, change it
    simS.frameSize = blockSize; // <-- same
    
    simS.maxNumSources = 1;               // du hast 1 Quelle
    simS.numThreads = 1;                  // oder std::thread::hardware_concurrency()

    simS.maxNumRays = 100000;             // muss >= shared.numRays (oder zumindest >0)
    simS.numDiffuseSamples = 64;          // typisch 32..128
    simS.maxNumOcclusionSamples = 16;     // >0, auch wenn du Occlusion nicht nutzt
    simS.maxDuration = 2.0f;              // muss >= shared.duration
    simS.numVisSamples = 8;               // >0 (für Probe visibility/pathing intern)

    // optional, aber oft sinnvoll:
    // simS.irradianceMinDistance = 1.0f;    // falls Feld existiert

    IPLSimulator sim = nullptr;
    check(iplSimulatorCreate(ctx, &simS, &sim), "iplSimulatorCreate");
    iplSimulatorSetScene(sim, scene);
    iplSimulatorCommit(sim);

    // ---- source
    IPLSourceSettings srcS{};
    srcS.flags = (IPLSimulationFlags)(IPL_SIMULATIONFLAGS_REFLECTIONS);

    // In 4.8, SourceSettings typically has more fields (rays/bounces/duration).
    // Fill them if they exist in your header; otherwise leave defaults.
    // Examples (rename fields to match your phonon.h if present):
    // srcS.numRays = 100000;
    // srcS.numBounces = 16;
    // srcS.numDiffuseSamples = 64;
    // srcS.maxDuration = irSeconds;

    IPLSource source = nullptr;
    check(iplSourceCreate(sim, &srcS, &source), "iplSourceCreate");

    // ---- positions (inside the box!)
    IPLVector3 srcPos{ 4.5f, 2.0f, 1.5f };
    IPLVector3 micPos{ 2.0f, 1.5f, 1.5f };

    // ---- reflection effect
    // Audio settings + effect settings
    IPLAudioSettings audioSettings{};
    // Adjust field names if needed:
    audioSettings.samplingRate = fs;
    audioSettings.frameSize = blockSize;

    IPLReflectionEffectSettings effS{};
    // Adjust field names/types if needed:
    effS.type = IPL_REFLECTIONEFFECTTYPE_CONVOLUTION;  // or HYBRID
    effS.numChannels = 4;                              // due to order = 1
    effS.irSize = (int)(irSeconds * fs);               // samples

    IPLReflectionEffect effect = nullptr;
    check(iplReflectionEffectCreate(ctx, &audioSettings, &effS, &effect), "iplReflectionEffectCreate");

    // Mixer: for mono mic you can usually pass nullptr.
    // If your validation complains, search phonon.h for iplReflectionMixerCreate.
    IPLReflectionMixer mixer = nullptr;

    // ---- buffers
    std::vector<float> out;
    out.resize((size_t)inFrames + (size_t)(irSeconds * fs) + 4096, 0.0f);

    std::vector<float> inBlock(blockSize, 0.0f);
    // std::vector<float> refBlock(blockSize, 0.0f);

    size_t inCursor = 0;
    size_t outCursor = 0;

    // We'll add direct at the end (whole-signal) for simplicity
    std::vector<float> inVec(inData, inData + inFrames);

    while (inCursor < (size_t)inFrames) 
    {
        int nThis = (int)std::min((size_t)blockSize, (size_t)inFrames - inCursor);
        std::fill(inBlock.begin(), inBlock.end(), 0.0f);
        std::copy(inData + inCursor, inData + inCursor + nThis, inBlock.begin());

        IPLCoordinateSpace3 srcCS{};
        srcCS.origin = srcPos;
        srcCS.right = IPLVector3{ 1,0,0 };
        srcCS.up = IPLVector3{ 0,0,1 };
        srcCS.ahead = IPLVector3{ 0,1,0 };

        // 1) Shared inputs FIRST
        // 
        IPLCoordinateSpace3 lisCS{};
        lisCS.origin = micPos;
        lisCS.right = { 1,0,0 };
        lisCS.up = { 0,0,1 };
        lisCS.ahead = { 0,1,0 };

        IPLSimulationSharedInputs shared{};
        shared.listener = lisCS; // ggf. shared.listenerCoordinates = lisCS;
        shared.numRays = 100000;
        shared.numBounces = 16;
        shared.duration = 2.0f;
        shared.order = 1;
        shared.irradianceMinDistance = 1.0f;

        iplSimulatorSetSharedInputs(sim, IPL_SIMULATIONFLAGS_REFLECTIONS, &shared);


        // 2) Then source inputs
        // --- simulation inputs
        IPLSimulationInputs inputs{};
        inputs.source = srcCS;  // IPLCoordinateSpace3 mit origin/right/up/ahead
        inputs.directFlags = (IPLDirectSimulationFlags)0;//IPL_DIRECTSIMULATIONFLAGS_NONE;   // <<< WICHTIG
        inputs.flags = (IPLSimulationFlags)(IPL_SIMULATIONFLAGS_REFLECTIONS /*| IPL_SIMULATIONFLAGS_DIRECT*/);
        
        for (int i = 0; i < IPL_NUM_BANDS; ++i)
            inputs.reverbScale[i] = 1.0f;

        inputs.hybridReverbTransitionTime = 1.0f;   // nur relevant bei HYBRID, aber harmless
        inputs.hybridReverbOverlapPercent = 0.25f;  // dito

        /*
        inputs.listener.origin = micPos;
        inputs.listener.ahead = { 1,0,0 };
        inputs.listener.up = { 0,0,1 };
        */

        // Modelle (für Start simple Defaults)
        inputs.distanceAttenuationModel.type = IPL_DISTANCEATTENUATIONTYPE_DEFAULT;
        inputs.airAbsorptionModel.type = IPL_AIRABSORPTIONTYPE_DEFAULT;
        inputs.directivity = { 0.0f, 1.0f };                   // omni
        inputs.occlusionType = IPL_OCCLUSIONTYPE_RAYCAST;
        inputs.occlusionRadius = 0.0f;
        inputs.numOcclusionSamples = 0;
        inputs.baked = IPL_FALSE;
        inputs.pathingProbes = nullptr;

        iplSourceSetInputs(source, inputs.flags, &inputs);

        
        // run reflections sim for this frame
        iplSimulatorRunReflections(sim);

        IPLSimulationOutputs outputs{};
        // outputs.reflections.numChannels = 1;
        iplSourceGetOutputs(source, IPL_SIMULATIONFLAGS_REFLECTIONS, &outputs);

        printf("refl.type=%d irSize=%d numCh=%d\n",
            (int)outputs.reflections.type,
            outputs.reflections.irSize,
            outputs.reflections.numChannels);

        // Debug sofort:
        printf("irSize=%d numCh=%d type=%d\n",
            outputs.reflections.irSize, outputs.reflections.numChannels, outputs.reflections.type);

        // AudioBuffer bauen
        float* inCh[1] = { inBlock.data() };

        std::vector<float> refW(blockSize, 0.0f);
        std::vector<float> refX(blockSize, 0.0f);
        std::vector<float> refY(blockSize, 0.0f);
        std::vector<float> refZ(blockSize, 0.0f);

        float* outCh[4] = { refW.data(), refX.data(), refY.data(), refZ.data() };
        // float* outCh[1] = { refBlock.data() };

        IPLAudioBuffer inBuf{};
        inBuf.numChannels = 1;
        inBuf.numSamples = blockSize;
        inBuf.data = inCh;

        IPLAudioBuffer outBuf{};
        outBuf.numChannels = 4; // 1
        outBuf.numSamples = blockSize;
        outBuf.data = outCh;

        std::fill(refW.begin(), refW.end(), 0.0f);
        std::fill(refX.begin(), refX.end(), 0.0f);
        std::fill(refY.begin(), refY.end(), 0.0f);
        std::fill(refZ.begin(), refZ.end(), 0.0f);

        // Effekt anwenden: outputs.reflections ist schon der richtige Param-Struct!
        iplReflectionEffectApply(effect, &outputs.reflections, &inBuf, &outBuf, mixer);

        float rms = 0.f;
        for (float v : refW) rms += v * v;
        rms = std::sqrt(rms / (float)refW.size());
        printf("ref RMS=%g\n", rms);


        // Mix reflections into output
        for (int i = 0; i < nThis; ++i) 
        {
            if (outCursor + i < out.size())
            {
                // out[outCursor + i] += refBlock[i];
                out[outCursor + i] += refW[i];
            }
        }

        inCursor += (size_t)nThis;
        outCursor += (size_t)nThis;
    }

    // Add analytic direct path
    // addDirectWhole(out, inVec, fs, srcPos, micPos);

    // Tail after input end
    std::vector<float> tailBlock(blockSize, 0.0f);
    while (true) {
        std::fill(tailBlock.begin(), tailBlock.end(), 0.0f);
        float* tailCh[1] = { tailBlock.data() };

        IPLAudioBuffer tailBuf{};
        tailBuf.numChannels = 1;
        tailBuf.numSamples = blockSize;
        tailBuf.data = tailCh;

        IPLAudioEffectState st = iplReflectionEffectGetTail(effect, &tailBuf, mixer);
        if (st == IPL_AUDIOEFFECTSTATE_TAILCOMPLETE)  // enum name in phonon.h
            break;

        for (int i = 0; i < blockSize; ++i) {
            if (outCursor + (size_t)i < out.size())
                out[outCursor + (size_t)i] += tailBlock[i];
        }
        outCursor += (size_t)blockSize;
    }

    // ---- write output wav (mono float32)
    drwav_data_format fmt{};
    fmt.container = drwav_container_riff;
    fmt.format = DR_WAVE_FORMAT_IEEE_FLOAT;
    fmt.channels = 1;
    fmt.sampleRate = (drwav_uint32)fs;
    fmt.bitsPerSample = 32;

    drwav wav{};
    if (!drwav_init_file_write(&wav, outPath, &fmt, nullptr)) {
        std::printf("Failed to write %s\n", outPath);
        return 1;
    }

    // Optional: normalize a bit for listening (comment out if you want raw)
    // float maxAbs = 0.f;
    // for (size_t i=0;i<outCursor;i++) maxAbs = std::max(maxAbs, std::fabs(out[i]));
    // if (maxAbs > 0.f) { float s = 0.9f / maxAbs; for (size_t i=0;i<outCursor;i++) out[i] *= s; }

    drwav_write_pcm_frames(&wav, (drwav_uint64)outCursor, out.data());
    drwav_uninit(&wav);

    drwav_free(inData, nullptr);

    // ---- cleanup
    iplReflectionEffectRelease(&effect);
    iplSourceRelease(&source);
    iplSimulatorRelease(&sim);
    iplStaticMeshRelease(&mesh);
    iplSceneRelease(&scene);
    iplContextRelease(&ctx);

    std::printf("Wrote %s\n", outPath);
    return 0;
}

