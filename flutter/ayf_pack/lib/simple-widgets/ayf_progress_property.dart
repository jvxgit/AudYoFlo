import 'package:flutter/material.dart';
import '../../ayf_pack_local.dart';
import 'package:provider/provider.dart';

class AudYoFloProgressIndicator extends StatefulWidget {
  final bool updateRealtime;
  final JvxComponentIdentification cpId;
  final double progressValue;
  final String propNameProgress;

  const AudYoFloProgressIndicator(this.cpId, this.propNameProgress,
      this.updateRealtime, this.progressValue);
  @override
  _AudYoFloProgressIndicatorState createState() =>
      _AudYoFloProgressIndicatorState();
}

class _AudYoFloProgressIndicatorState extends State<AudYoFloProgressIndicator> {
  double progressValue = 0;
  int oldHashCode = -1;
  @override
  Widget build(BuildContext context) {
    return Selector<AudYoFloPeriodicNotifier, bool>(
        selector: (context, perNotify) => !perNotify.fast,
        shouldRebuild: (previous, next) {
          if (widget.updateRealtime) {
            return next ? true : false;
          }
          return false;
        },
        builder: (context, notifier, child) {
          // Check if we really have allocated a new widget
          if (widget.hashCode != oldHashCode) {
            progressValue = widget.progressValue;
            oldHashCode = widget.hashCode;
          }

          // double progressValueLocal = widget.progressValue;
          var theBeCache =
              Provider.of<AudYoFloBackendCache>(context, listen: false);
          List<String> lst = [widget.propNameProgress];

          List<AudYoFloPropertyContainer>? props =
              theBeCache.referenceValidPropertiesComponents(widget.cpId, lst);
          if (props == null) {
            // No need to invoke a listener - we wait for the operation to complete
            theBeCache.triggerUpdatePropertiesComponent(widget.cpId, lst,
                report: AyfPropertyReportLevel.AYF_FRONTEND_REPORT_NO_REPORT);
          } else {
            // Update progressValue
            if (props.isNotEmpty) {
              AudYoFloPropertyContainer prop = props.first;
              if (prop is AudYoFloPropertyMultiContentBackend) {
                AudYoFloPropertyMultiContentBackend propN =
                    prop as AudYoFloPropertyMultiContentBackend;
                progressValue = propN.fld[0];
                theBeCache.invalidatePropertiesComponent(
                    widget.cpId, lst, true);
              }
            }
          }

          // Convert percent to value between 0 and 1
          double progressValueShow = progressValue / 100;
          return LinearProgressIndicator(value: progressValueShow);
        });
  }
}
