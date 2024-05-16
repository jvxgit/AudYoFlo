import '../ayf_pack_local.dart';

abstract class AudYoFloDirectPropertiesIf {
  int setProperty(AudYoFloPropertyContainer prop);
  int getProperty(AudYoFloPropertyContainer prop);
  int getPropertyDescriptors(List<AudYoFloPropertyContainer> propsOnReturn);
}
