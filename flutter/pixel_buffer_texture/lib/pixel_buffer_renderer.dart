import 'package:flutter/material.dart';
import 'package:pixel_buffer_texture/pixel_buffer_texture_platform_interface.dart';
import 'package:ayf_pack/ayf_pack.dart';

typedef PixelBufferRendererOnRenderCallback = void Function(
    int width, int height);

class PixelBufferRenderer extends StatefulWidget {
  final int width;
  final int height;
  final String rendererId;
  final String rendererCtxt;
  final String rendererArg;

  const PixelBufferRenderer(
      {required this.width,
      required this.height,
      required this.rendererId,
      required this.rendererCtxt,
      required this.rendererArg,
      super.key});

  @override
  State<PixelBufferRenderer> createState() => _PixelBufferRendererState();
}

class _PixelBufferRendererState extends State<PixelBufferRenderer> {
  Future<int>? _texture;

  @override
  void initState() {
    super.initState();
    _texture = PixelBufferTexturePlatform.instance.createTextureRenderer(
        widget.width,
        widget.height,
        widget.rendererId,
        widget.rendererCtxt,
        widget.rendererArg);
  }

  @override
  void dispose() {
    _texture?.then((textureId) =>
        PixelBufferTexturePlatform.instance.destroyTextureRenderer(textureId));
    super.dispose();
  }

  void local_entries() async {
    var entries = await PixelBufferTexturePlatform.instance.getEntryPoints();
    print('Entries: $entries');
  }

  @override
  Widget build(BuildContext context) {
    local_entries();

    return SizedBox(
        width: widget.width.toDouble(),
        height: widget.height.toDouble(),
        child: FutureBuilder(
            future: _texture,
            builder: (context, snapshot) {
              if (snapshot.hasError || !snapshot.hasData) {
                return const SizedBox.shrink();
              }
              return ColoredBox(
                  color: Colors.black,
                  child: Texture(textureId: snapshot.data!));
            }));
  }
}

class Ayfpixbufpack extends AyfCorePackIf {
  @override
  Future<Map<dynamic, dynamic>?> getEntryPoints() {
    return PixelBufferTexturePlatform.instance.getEntryPoints();
  }
}

Ayfpixbufpack get ayfpixbuf => Ayfpixbufpack();
