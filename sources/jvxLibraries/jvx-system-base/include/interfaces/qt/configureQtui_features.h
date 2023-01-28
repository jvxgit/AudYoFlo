#ifndef __CONFIGUREQTUI_FEATURES_H__
#define __CONFIGUREQTUI_FEATURES_H__

typedef enum
{
	JVX_QT_MAINWIDGET_SCROLL,
	JVX_QT_MAINWIDGET_EXPAND_FULLSIZE,
	JVX_QT_MAINWIDGET_NO_EXPAND_CENTER
} jvxQtWidgetTweakEnum;

class configureQtui_features
{
public:

	struct
	{
		jvxInt32 minWidthWindow;
		jvxInt32 minHeightWindow;
		jvxInt32 maxWidthWindow;
		jvxInt32 maxHeightWindow;

		jvxBool smallicons;
		std::string endingicons;
		std::string smallposticons;
		std::string foldericons;
		jvxSize iconw_small;
		jvxSize iconh_small;
		jvxSize iconw_normal;
		jvxSize iconh_normal;

		jvxData scalefac_splash;
		int ftsize_splash;
		jvxBool italic_splash;
		const char* pixmap_splash;
		
		jvxQtWidgetTweakEnum tweakUi;
	}config_ui;

	jvxInt32 timeout_viewer_maincentral_ms; // Default: 500	
	jvxInt32 rtv_period_props_ms;
	jvxInt32 rtv_period_plots_ms;

	jvxBool acceptSpaceStart;

	configureQtui_features()
	{
		config_ui.smallicons = true;
		config_ui.endingicons = "png";
		config_ui.smallposticons = "-small";
		config_ui.foldericons = ":/images/images/";
		config_ui.iconw_small = 50;
		config_ui.iconh_small = 50;
		config_ui.iconw_normal = 134;
		config_ui.iconh_normal = 50;
		config_ui.scalefac_splash = 0.8;
		config_ui.pixmap_splash = ":/images/images/splash.png";
		config_ui.ftsize_splash = 24;
		config_ui.italic_splash = true;
		timeout_viewer_maincentral_ms = 500;
		rtv_period_props_ms = 500;
		rtv_period_plots_ms = 500;
		config_ui.tweakUi = JVX_QT_MAINWIDGET_EXPAND_FULLSIZE;
		config_ui.minHeightWindow = -1;
		config_ui.minWidthWindow = -1;
		config_ui.maxHeightWindow = -1;
		config_ui.maxWidthWindow = -1;

		acceptSpaceStart = true;
	};
};

#endif
