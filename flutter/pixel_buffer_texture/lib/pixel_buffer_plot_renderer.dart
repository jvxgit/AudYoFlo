import 'package:flutter/material.dart';
import 'package:flutter/scheduler.dart';

import 'pixel_buffer_plot_config.dart';
import 'pixel_buffer_texture_platform_interface.dart';

export 'pixel_buffer_plot_config.dart';

/// Shows a 2D plot buffer of a backend component (e.g. the real-time
/// spectrogram of jvxAuNAnalyze), rendered in the backend with
/// jvxVideoRenderCore - the counterpart of CjvxMyGlWidget in the Qt hosts.
///
/// The backend installs the external buffer in the rendering target property
/// when processing starts and removes it when processing stops. In between,
/// the plot follows the buffer content in real-time.
///
/// The texture is rendered in physical pixels and follows the widget size.
class PixelBufferPlotRenderer extends StatefulWidget {
  /// Component holding the rendering target (JvxComponentIdentification.txt)
  final String rendererId;

  /// Association hint stored in the external buffer
  final String rendererCtxt;

  /// "<install target property>[:<key>=<value>]*" with mode=plot_inc or
  /// mode=plot_full, see TpjvxPixBuffer.h
  final String rendererArg;

  /// Overlay (axes, labels, title, legend)
  final PixelBufferPlotConfig config;

  const PixelBufferPlotRenderer({
    required this.rendererId,
    this.rendererCtxt = '',
    required this.rendererArg,
    this.config = const PixelBufferPlotConfig(),
    super.key,
  });

  @override
  State<PixelBufferPlotRenderer> createState() =>
      _PixelBufferPlotRendererState();
}

class _PixelBufferPlotRendererState extends State<PixelBufferPlotRenderer> {
  int _textureId = -1;
  bool _creating = false;
  bool _failed = false;
  bool _disposed = false;

  int _widthReq = 0;
  int _heightReq = 0;
  double _scaleReq = 1.0;

  int _widthSent = 0;
  int _heightSent = 0;
  double _scaleSent = 1.0;
  PixelBufferPlotConfig? _configSent;

  PixelBufferTexturePlatform get _platform =>
      PixelBufferTexturePlatform.instance;

  @override
  void didUpdateWidget(covariant PixelBufferPlotRenderer oldWidget) {
    super.didUpdateWidget(oldWidget);
    if ((oldWidget.rendererId != widget.rendererId) ||
        (oldWidget.rendererArg != widget.rendererArg) ||
        (oldWidget.rendererCtxt != widget.rendererCtxt)) {
      // Other rendering target: start from scratch
      _release();
      _failed = false;
      _schedule();
    } else if (widget.config != _configSent) {
      _syncConfig();
    }
  }

  @override
  void dispose() {
    _disposed = true;
    _release();
    super.dispose();
  }

  void _release() {
    if (_textureId >= 0) {
      _platform.destroyTextureRenderer(_textureId);
      _textureId = -1;
    }
    _widthSent = 0;
    _heightSent = 0;
    _configSent = null;
  }

  void _schedule() {
    SchedulerBinding.instance.addPostFrameCallback((_) => _sync());
  }

  Future<void> _sync() async {
    if (_disposed || _failed || (_widthReq <= 0) || (_heightReq <= 0)) {
      return;
    }

    if (_textureId < 0) {
      if (_creating) {
        return;
      }
      _creating = true;
      final w = _widthReq;
      final h = _heightReq;
      final id = await _platform.createTextureRenderer(
          w, h, widget.rendererId, widget.rendererCtxt, widget.rendererArg);
      _creating = false;
      if (_disposed) {
        if (id >= 0) {
          _platform.destroyTextureRenderer(id);
        }
        return;
      }
      if (id < 0) {
        setState(() => _failed = true);
        return;
      }
      _widthSent = w;
      _heightSent = h;
      _scaleSent = 0; // Force passing the scale factor
      setState(() => _textureId = id);
    }

    // Size or configuration may have changed in the meantime
    if ((_widthReq != _widthSent) ||
        (_heightReq != _heightSent) ||
        (_scaleReq != _scaleSent)) {
      _widthSent = _widthReq;
      _heightSent = _heightReq;
      _scaleSent = _scaleReq;
      await _platform.configureTextureRenderer(
          _textureId, 'size', '$_widthSent:$_heightSent:$_scaleSent');
    }
    _syncConfig();
  }

  void _syncConfig() {
    if ((_textureId >= 0) && (widget.config != _configSent)) {
      _configSent = widget.config;
      _platform.configureTextureRenderer(
          _textureId, 'config', widget.config.encode());
    }
  }

  @override
  Widget build(BuildContext context) {
    final dpr = MediaQuery.of(context).devicePixelRatio;
    return LayoutBuilder(builder: (context, constraints) {
      final w = (constraints.maxWidth.isFinite ? constraints.maxWidth : 400.0);
      final h =
          (constraints.maxHeight.isFinite ? constraints.maxHeight : 300.0);
      final wPhys = (w * dpr).round();
      final hPhys = (h * dpr).round();
      if ((wPhys != _widthReq) || (hPhys != _heightReq) || (dpr != _scaleReq)) {
        _widthReq = wPhys;
        _heightReq = hPhys;
        _scaleReq = dpr;
        _schedule();
      }

      Widget child;
      if (_textureId >= 0) {
        child =
            Texture(textureId: _textureId, filterQuality: FilterQuality.none);
      } else if (_failed) {
        child = const Center(
            child: Text('Plot rendering not available',
                style: TextStyle(color: Colors.white70)));
      } else {
        child = const SizedBox.shrink();
      }
      return SizedBox(
          width: w,
          height: h,
          child: ColoredBox(color: const Color(0xFF1A1A1A), child: child));
    });
  }
}
