#include <phonon.h>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include "jvx.h"

// Helper: einfache Fehlerprüfung
static void check(IPLerror err, const char* what)
{
    if (err != IPL_STATUS_SUCCESS) {
        std::printf("SteamAudio error %d at %s\n", (int)err, what);
        std::exit(1);
    }
}

// Helper: 12 Dreiecke für eine Box (innenflächen) – sehr simple Version.
// Koordinaten: Box von (0,0,0) bis (Lx,Ly,Lz). Winding ist wichtig (CCW). :contentReference[oaicite:12]{index=12}
static void buildShoebox(float Lx, float Ly, float Lz,
                         std::vector<IPLVector3>& V,
                         std::vector<IPLTriangle>& T)
{
    V = {
        {0,  0,  0},   {Lx, 0,  0},   {Lx, Ly, 0},   {0,  Ly, 0},   // z=0
        {0,  0,  Lz},  {Lx, 0,  Lz},  {Lx, Ly, Lz},  {0,  Ly, Lz}   // z=Lz
    };

    // 6 Flächen * 2 Triangles = 12
    // Boden (z=0)
    T.push_back({0, 2, 1}); T.push_back({0, 3, 2});
    // Decke (z=Lz)
    T.push_back({4, 5, 6}); T.push_back({4, 6, 7});
    // Wand y=0
    T.push_back({0, 1, 5}); T.push_back({0, 5, 4});
    // Wand y=Ly
    T.push_back({3, 6, 2}); T.push_back({3, 7, 6});
    // Wand x=0
    T.push_back({0, 4, 7}); T.push_back({0, 7, 3});
    // Wand x=Lx
    T.push_back({1, 2, 6}); T.push_back({1, 6, 5});
}

int main()
{
    std::cout << "Starting generation of impulse response for showbox room!" << std::endl;

    // ---- 1) Context
    IPLContextSettings cs{};
    cs.version = STEAMAUDIO_VERSION;

    IPLContext ctx = nullptr;
    check(iplContextCreate(&cs, &ctx), "iplContextCreate"); // :contentReference[oaicite:13]{index=13}

    // ---- 2) Scene
    IPLSceneSettings ss{};
    ss.type = IPL_SCENETYPE_DEFAULT;

    IPLScene scene = nullptr;
    check(iplSceneCreate(ctx, &ss, &scene), "iplSceneCreate"); // :contentReference[oaicite:14]{index=14}

    // ---- 3) Shoebox-Mesh -> StaticMesh
    const float Lx = 6.0f, Ly = 4.0f, Lz = 3.0f;

    std::vector<IPLVector3> vertices;
    std::vector<IPLTriangle> triangles;

    std::cout << "Running function to build showbox room with Lx = " << Lx << ", Ly = " << Ly << ", Lz = " << Lz << std::endl;
    buildShoebox(Lx, Ly, Lz, vertices, triangles);

    // 1 Material für alles (Absorption/Scattering/Transmission). Beispielwerte.
    IPLMaterial mats[1] = {
        { {0.2f, 0.2f, 0.2f}, 0.1f, {0.0f, 0.0f, 0.0f} }
    };
    std::vector<IPLint32> matIdx(triangles.size(), 0);

    IPLStaticMeshSettings ms{};
    ms.numVertices = (IPLint32)vertices.size();
    ms.numTriangles = (IPLint32)triangles.size();
    ms.numMaterials = 1;
    ms.vertices = vertices.data();
    ms.triangles = triangles.data();
    ms.materialIndices = matIdx.data();
    ms.materials = mats;

    IPLStaticMesh mesh = nullptr;
    check(iplStaticMeshCreate(scene, &ms, &mesh), "iplStaticMeshCreate"); // :contentReference[oaicite:15]{index=15}

    iplStaticMeshAdd(mesh, scene);
    iplSceneCommit(scene); // Änderungen aktivieren :contentReference[oaicite:16]{index=16}

    // ---- 4) ProbeBatch mit 1 Probe am Mikrofon-Ort
    IPLProbeBatch probes = nullptr;
    check(iplProbeBatchCreate(ctx, &probes), "iplProbeBatchCreate"); // :contentReference[oaicite:17]{index=17}

    IPLSphere probeSphere{};
    probeSphere.center = IPLVector3{ 2.0f, 1.5f, 1.5f }; // Mic-Position
    probeSphere.radius = 0.1f;
    iplProbeBatchAddProbe(probes, probeSphere);
    iplProbeBatchCommit(probes); // :contentReference[oaicite:18]{index=18}

    // ---- 5) Reflections bake (statische Quelle)
    // Quelle:
    const IPLVector3 sourcePos{ 4.5f, 2.0f, 1.5f };

    IPLBakedDataIdentifier id{};
    id.type = IPL_BAKEDDATATYPE_REFLECTIONS;
    id.variation = IPL_BAKEDDATAVARIATION_STATICSOURCE;
    id.endpointInfluence.center = sourcePos;
    id.endpointInfluence.radius = 0.1f;

    IPLReflectionsBakeParams bp{};
    bp.scene = scene;
    bp.probeBatch = probes;
    bp.sceneType = ss.type;
    bp.identifier = id;
    bp.bakeFlags = (IPLReflectionsBakeFlags)(IPL_REFLECTIONSBAKEFLAGS_BAKECONVOLUTION);
    bp.numRays = 20000;
    bp.numDiffuseSamples = 32;
    bp.numBounces = 8;

    bp.simulatedDuration = 2.0f; // Sekunden :contentReference[oaicite:19]{index=19}
    bp.savedDuration = 2.0f;     // Sekunden :contentReference[oaicite:20]{index=20}
    bp.order = 0;                // 0 = mono IR (für 1 Mikro am einfachsten) :contentReference[oaicite:21]{index=21}
    bp.numThreads = 0;           // 0 = Steam Audio entscheidet

    iplReflectionsBakerBake(ctx, &bp, nullptr, nullptr); // :contentReference[oaicite:22]{index=22}

    // ---- 6) EnergyField holen
    IPLEnergyFieldSettings efs{};
    efs.duration = bp.savedDuration;
    efs.order = bp.order;

    IPLEnergyField ef = nullptr;
    check(iplEnergyFieldCreate(ctx, &efs, &ef), "iplEnergyFieldCreate"); // :contentReference[oaicite:23]{index=23}

    iplProbeBatchGetEnergyField(probes, &id, 0, ef); // Probe index 0 :contentReference[oaicite:24]{index=24}

    // ---- 7) Reconstruct -> ImpulseResponse
    IPLReconstructorSettings rs{};
    rs.maxDuration = bp.savedDuration;
    rs.maxOrder = bp.order;
    rs.samplingRate = 48000;

    IPLReconstructor recon = nullptr;
    check(iplReconstructorCreate(ctx, &rs, &recon), "iplReconstructorCreate"); // :contentReference[oaicite:25]{index=25}

    IPLImpulseResponseSettings irs{};
    irs.duration = bp.savedDuration;
    irs.order = bp.order;
    irs.samplingRate = rs.samplingRate;

    IPLImpulseResponse ir = nullptr;
    check(iplImpulseResponseCreate(ctx, &irs, &ir), "iplImpulseResponseCreate"); // :contentReference[oaicite:26]{index=26}

    IPLReconstructorInputs in{};
    in.energyField = ef;

    IPLReconstructorSharedInputs shared{};
    shared.duration = bp.savedDuration;
    shared.order = bp.order;

    IPLReconstructorOutputs out{};
    out.impulseResponse = ir;

    iplReconstructorReconstruct(recon, 1, &in, &shared, &out); // :contentReference[oaicite:27]{index=27}

    // ---- 8) Samples rausziehen (order=0 => 1 Kanal)
    const int nCh = iplImpulseResponseGetNumChannels(ir);
    const int nSm = iplImpulseResponseGetNumSamples(ir);
    float* data = iplImpulseResponseGetData(ir); // row-major: ch*samples :contentReference[oaicite:28]{index=28}

    std::printf("IR: channels=%d samples=%d\n", nCh, nSm);
    std::cout << "First 100 samples of ir = [" << std::flush;
    for (int i = 0; i < 100;i++)
    {
        if (i > 0)
        {
            std::cout << ", " << std::flush;
        }
        std::cout << data[i] << std::flush;
    }
    std::cout << "];" << std::endl;

    std::vector<float> irMono;
    irMono.assign(data, data + nSm); // Kanal 0

    // TODO: irMono als WAV speichern (32-bit float) oder weiterverarbeiten.

    // ---- Cleanup (Release ist refcounted) :contentReference[oaicite:29]{index=29}
    iplImpulseResponseRelease(&ir);
    iplReconstructorRelease(&recon);
    iplEnergyFieldRelease(&ef);
    iplProbeBatchRelease(&probes);
    // mesh/scene/context ebenfalls releasen:
    iplStaticMeshRelease(&mesh);
    iplSceneRelease(&scene);
    iplContextRelease(&ctx);

    return 0;
}
