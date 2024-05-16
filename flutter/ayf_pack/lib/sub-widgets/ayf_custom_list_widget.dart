import 'package:flutter/material.dart';

//
// https://docs.flutter.dev/development/ui/advanced/slivers
//
class AudYoFloCustomListWidget extends StatefulWidget {
  @override
  State<StatefulWidget> createState() {
    return _AudYoFloCustomListWidgetStates();
  }
}

class _AudYoFloCustomListWidgetStates extends State<AudYoFloCustomListWidget> {
  ScrollController _myControllerL = ScrollController();
  ScrollController _myControllerR = ScrollController();
  @override
  Widget build(BuildContext context) {
    return Row(children: [
      Flexible(
        flex: 1,
        child: ListView.builder(
          controller: _myControllerL,
          itemCount: 10,
          itemBuilder: (BuildContext context, int index) {
            return Card(
                child: Row(
                    mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                    children: [
                  Padding(
                    padding: const EdgeInsets.all(8.0),
                    child: SizedBox(
                        height: 200,
                        width: 200,
                        child: Stack(children: [
                          Image.asset(
                              'images/fernlive_icons/fernlive-hdrive.png'),
                          Text("--")
                        ])),
                  ),
                  Text(index.toString()),
                  Text('--')
                ]));
          },
        ),
      ),
      Flexible(
        flex: 1,
        child: CustomScrollView(
          controller: _myControllerR,
          slivers: [
            SliverAppBar(
                title: Center(child: Text('Audio Devices')),
                backgroundColor: Colors.blueGrey,
                pinned: true,
                stretch: true,
                onStretchTrigger: () async {
                  print('Hello');
                },
                floating: true,
                snap: true,
                expandedHeight: 200),
            SliverList(
              delegate:
                  SliverChildBuilderDelegate((BuildContext context, int index) {
                return Card(
                    child: Row(
                        mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                        children: [
                      Padding(
                        padding: const EdgeInsets.all(8.0),
                        child: SizedBox(
                            height: 200,
                            width: 200,
                            child: Stack(children: [
                              Image.asset(
                                  'images/fernlive_icons/fernlive-hdrive.png'),
                              Text("--")
                            ])),
                      ),
                      Text(index.toString()),
                      Text('--')
                    ]));
              }, childCount: 10),
            )
          ],
        ),
      ),
      Text('xy')
    ]);
  }
}
