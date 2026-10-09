set(PROJECT_NAME "qwt")
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(QWT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/qwt)

set(QWT_URL
    "https://sourceforge.net/projects/qwt/files/qwt/6.3.0/qwt-6.3.0.zip/download"
)

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/qwt)

	file(DOWNLOAD
		"${QWT_URL}"
		"${CMAKE_CURRENT_SOURCE_DIR}/qwt-6.3.0.zip"
		SHOW_PROGRESS
		STATUS download_status
		)

	file(ARCHIVE_EXTRACT
		INPUT "${CMAKE_CURRENT_SOURCE_DIR}/qwt-6.3.0.zip"
		DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}"
		)

	file(RENAME
       "${CMAKE_CURRENT_SOURCE_DIR}/qwt-6.3.0"
       "${QWT_DIR}")
	   
	file(REMOVE "${CMAKE_CURRENT_SOURCE_DIR}/qwt-6.3.0.zip")
	
endif()
	   
#if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/qwt)
	#message("qwt sub project not available. Starting script to download it.") 
	#execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules.sh; exit")
	#message("qwt installation complete.")
#endif()

set(CMAKE_AUTOMOC ON)
set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};QWT_MOC_INCLUDE")

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../qwt/src
)

set(
  LOCAL_SOURCES 
  ${QWT_DIR}/src/qwt.cpp
  ${QWT_DIR}/src/qwt_abstract_legend.cpp
  ${QWT_DIR}/src/qwt_abstract_scale.cpp
  ${QWT_DIR}/src/qwt_abstract_scale_draw.cpp
  ${QWT_DIR}/src/qwt_abstract_slider.cpp
  ${QWT_DIR}/src/qwt_analog_clock.cpp
  ${QWT_DIR}/src/qwt_arrow_button.cpp
  ${QWT_DIR}/src/qwt_bezier.cpp
  ${QWT_DIR}/src/qwt_clipper.cpp
  ${QWT_DIR}/src/qwt_color_map.cpp
  ${QWT_DIR}/src/qwt_column_symbol.cpp
  ${QWT_DIR}/src/qwt_compass.cpp
  ${QWT_DIR}/src/qwt_compass_rose.cpp
  ${QWT_DIR}/src/qwt_counter.cpp
  ${QWT_DIR}/src/qwt_curve_fitter.cpp
  ${QWT_DIR}/src/qwt_date.cpp
  ${QWT_DIR}/src/qwt_date_scale_draw.cpp
  ${QWT_DIR}/src/qwt_date_scale_engine.cpp
  ${QWT_DIR}/src/qwt_dial.cpp
  ${QWT_DIR}/src/qwt_dial_needle.cpp
  ${QWT_DIR}/src/qwt_dyngrid_layout.cpp
  ${QWT_DIR}/src/qwt_event_pattern.cpp
  ${QWT_DIR}/src/qwt_graphic.cpp
  ${QWT_DIR}/src/qwt_interval.cpp
  ${QWT_DIR}/src/qwt_interval_symbol.cpp
  ${QWT_DIR}/src/qwt_knob.cpp
  ${QWT_DIR}/src/qwt_legend.cpp
  ${QWT_DIR}/src/qwt_legend_data.cpp
  ${QWT_DIR}/src/qwt_legend_label.cpp
  ${QWT_DIR}/src/qwt_magnifier.cpp
  ${QWT_DIR}/src/qwt_math.cpp
  ${QWT_DIR}/src/qwt_matrix_raster_data.cpp
  ${QWT_DIR}/src/qwt_null_paintdevice.cpp
  ${QWT_DIR}/src/qwt_painter.cpp
  ${QWT_DIR}/src/qwt_painter_command.cpp
  ${QWT_DIR}/src/qwt_panner.cpp
  ${QWT_DIR}/src/qwt_picker.cpp
  ${QWT_DIR}/src/qwt_picker_machine.cpp
  ${QWT_DIR}/src/qwt_pixel_matrix.cpp
  ${QWT_DIR}/src/qwt_plot.cpp
  ${QWT_DIR}/src/qwt_plot_abstract_barchart.cpp
  ${QWT_DIR}/src/qwt_plot_abstract_canvas.cpp
  ${QWT_DIR}/src/qwt_plot_axis.cpp
  ${QWT_DIR}/src/qwt_plot_barchart.cpp
  ${QWT_DIR}/src/qwt_plot_canvas.cpp
  ${QWT_DIR}/src/qwt_plot_curve.cpp
  ${QWT_DIR}/src/qwt_plot_dict.cpp
  ${QWT_DIR}/src/qwt_plot_directpainter.cpp
  # ${QWT_DIR}/src/qwt_plot_glcanvas.cpp
  ${QWT_DIR}/src/qwt_plot_graphicitem.cpp
  ${QWT_DIR}/src/qwt_plot_grid.cpp
  ${QWT_DIR}/src/qwt_plot_histogram.cpp
  ${QWT_DIR}/src/qwt_plot_intervalcurve.cpp
  ${QWT_DIR}/src/qwt_plot_item.cpp
  ${QWT_DIR}/src/qwt_plot_layout.cpp
  ${QWT_DIR}/src/qwt_plot_legenditem.cpp
  ${QWT_DIR}/src/qwt_plot_magnifier.cpp
  ${QWT_DIR}/src/qwt_plot_marker.cpp
  ${QWT_DIR}/src/qwt_plot_multi_barchart.cpp
  ${QWT_DIR}/src/qwt_plot_opengl_canvas.cpp
  ${QWT_DIR}/src/qwt_plot_panner.cpp
  ${QWT_DIR}/src/qwt_plot_picker.cpp
  ${QWT_DIR}/src/qwt_plot_rasteritem.cpp
  ${QWT_DIR}/src/qwt_plot_renderer.cpp
  ${QWT_DIR}/src/qwt_plot_rescaler.cpp
  ${QWT_DIR}/src/qwt_plot_scaleitem.cpp
  ${QWT_DIR}/src/qwt_plot_seriesitem.cpp
  ${QWT_DIR}/src/qwt_plot_shapeitem.cpp
  ${QWT_DIR}/src/qwt_plot_spectrocurve.cpp
  ${QWT_DIR}/src/qwt_plot_spectrogram.cpp
  ${QWT_DIR}/src/qwt_plot_svgitem.cpp
  ${QWT_DIR}/src/qwt_plot_textlabel.cpp
  ${QWT_DIR}/src/qwt_plot_tradingcurve.cpp
  ${QWT_DIR}/src/qwt_plot_vectorfield.cpp
  ${QWT_DIR}/src/qwt_plot_zoneitem.cpp
  ${QWT_DIR}/src/qwt_plot_zoomer.cpp
  ${QWT_DIR}/src/qwt_point_3d.cpp
  ${QWT_DIR}/src/qwt_point_data.cpp
  ${QWT_DIR}/src/qwt_point_mapper.cpp
  ${QWT_DIR}/src/qwt_point_polar.cpp
  ${QWT_DIR}/src/qwt_polar_canvas.cpp
  ${QWT_DIR}/src/qwt_polar_curve.cpp
  ${QWT_DIR}/src/qwt_polar_fitter.cpp
  ${QWT_DIR}/src/qwt_polar_grid.cpp
  ${QWT_DIR}/src/qwt_polar_item.cpp
  ${QWT_DIR}/src/qwt_polar_itemdict.cpp
  ${QWT_DIR}/src/qwt_polar_layout.cpp
  ${QWT_DIR}/src/qwt_polar_magnifier.cpp
  ${QWT_DIR}/src/qwt_polar_marker.cpp
  ${QWT_DIR}/src/qwt_polar_panner.cpp
  ${QWT_DIR}/src/qwt_polar_picker.cpp
  ${QWT_DIR}/src/qwt_polar_plot.cpp
  ${QWT_DIR}/src/qwt_polar_renderer.cpp
  ${QWT_DIR}/src/qwt_polar_spectrogram.cpp
  ${QWT_DIR}/src/qwt_raster_data.cpp
  ${QWT_DIR}/src/qwt_round_scale_draw.cpp
  ${QWT_DIR}/src/qwt_sampling_thread.cpp
  ${QWT_DIR}/src/qwt_scale_div.cpp
  ${QWT_DIR}/src/qwt_scale_draw.cpp
  ${QWT_DIR}/src/qwt_scale_engine.cpp
  ${QWT_DIR}/src/qwt_scale_map.cpp
  ${QWT_DIR}/src/qwt_scale_widget.cpp
  ${QWT_DIR}/src/qwt_series_data.cpp
  ${QWT_DIR}/src/qwt_slider.cpp
  ${QWT_DIR}/src/qwt_spline.cpp
  ${QWT_DIR}/src/qwt_spline_basis.cpp
  ${QWT_DIR}/src/qwt_spline_cubic.cpp
  ${QWT_DIR}/src/qwt_spline_curve_fitter.cpp
  ${QWT_DIR}/src/qwt_spline_local.cpp
  ${QWT_DIR}/src/qwt_spline_parametrization.cpp
  ${QWT_DIR}/src/qwt_spline_pleasing.cpp
  ${QWT_DIR}/src/qwt_spline_polynomial.cpp
  ${QWT_DIR}/src/qwt_symbol.cpp
  ${QWT_DIR}/src/qwt_system_clock.cpp
  ${QWT_DIR}/src/qwt_text.cpp
  ${QWT_DIR}/src/qwt_text_engine.cpp
  ${QWT_DIR}/src/qwt_text_label.cpp
  ${QWT_DIR}/src/qwt_thermo.cpp
  ${QWT_DIR}/src/qwt_transform.cpp
  ${QWT_DIR}/src/qwt_vectorfield_symbol.cpp
  ${QWT_DIR}/src/qwt_weeding_curve_fitter.cpp
  ${QWT_DIR}/src/qwt_wheel.cpp
  ${QWT_DIR}/src/qwt_widget_overlay.cpp
  )  

set(BUILD_STATIC TRUE)
set(LOCAL_LIBS ${JVX_QT_LIBS})
include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)

