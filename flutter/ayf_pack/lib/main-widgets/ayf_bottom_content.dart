import 'package:flutter/material.dart';

class AudYoFloBottomContent extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return Center(
      child: Row(children: [
        ButtonBar(children: [
          ElevatedButton(child: Container(color: Colors.red), onPressed: () {})
        ]),
        Container(width: 120, height: 120, color: Colors.red),
        Container(color: Colors.green),
        Container(color: Colors.blue)
      ]),
    );
  }
}
