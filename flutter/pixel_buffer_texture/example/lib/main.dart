import 'package:flutter/material.dart';

import 'package:pixel_buffer_texture/pixel_buffer_renderer.dart';
import 'package:pixel_buffer_texture/pixel_buffer_texture_platform_interface.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({super.key});

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  @override
  void initState() {
    super.initState();
  }

  @override
  Widget build(BuildContext context) {
    const String rendererId = "ayfvidisp";
    const String rendererArg = '{""}';
    const String rendererCtxt = '{""}';
    return const MaterialApp(
      home: Scaffold(
        body: Center(
            child: PixelBufferRenderer(
                width: 640,
                height: 480,
                rendererId: rendererId,
                rendererArg: rendererArg,
                rendererCtxt: rendererCtxt,)),
      ),
    );
  }
}
