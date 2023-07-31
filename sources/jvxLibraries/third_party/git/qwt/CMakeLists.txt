
set(PROJECT_NAME "qwt")
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/qwt)
	message("qwt sub project not available. Starting script to download it.") 
	execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules.sh; exit")
	message("qwt installation complete.")
endif()

set(CMAKE_AUTOMOC ON)
set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};QWT_MOC_INCLUDE")

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../qwt/src
)

set(
  LOCAL_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_abstract_legend.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_abstract_scale.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_abstract_scale_draw.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_abstract_slider.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_analog_clock.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_arrow_button.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_bezier.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_clipper.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_color_map.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_column_symbol.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_compass.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_compass_rose.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_counter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_curve_fitter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_date.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_date_scale_draw.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_date_scale_engine.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_dial.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_dial_needle.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_dyngrid_layout.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_event_pattern.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_graphic.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_interval.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_interval_symbol.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_knob.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_legend.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_legend_data.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_legend_label.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_magnifier.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_math.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_matrix_raster_data.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_null_paintdevice.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_painter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_painter_command.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_panner.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_picker.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_picker_machine.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_pixel_matrix.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_abstract_barchart.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_abstract_canvas.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_axis.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_barchart.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_canvas.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_curve.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_dict.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_directpainter.cpp
  # ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_glcanvas.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_graphicitem.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_grid.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_histogram.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_intervalcurve.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_item.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_layout.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_legenditem.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_magnifier.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_marker.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_multi_barchart.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_opengl_canvas.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_panner.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_picker.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_rasteritem.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_renderer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_rescaler.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_scaleitem.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_seriesitem.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_shapeitem.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_spectrocurve.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_spectrogram.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_svgitem.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_textlabel.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_tradingcurve.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_vectorfield.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_zoneitem.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_zoomer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_point_3d.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_point_data.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_point_mapper.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_point_polar.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_polar_canvas.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_polar_curve.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_polar_fitter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_polar_grid.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_polar_item.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_polar_itemdict.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_polar_layout.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_polar_magnifier.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_polar_marker.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_polar_panner.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_polar_picker.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_polar_plot.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_polar_renderer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_polar_spectrogram.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_raster_data.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_round_scale_draw.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_sampling_thread.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_scale_div.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_scale_draw.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_scale_engine.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_scale_map.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_scale_widget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_series_data.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_slider.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_spline.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_spline_basis.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_spline_cubic.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_spline_curve_fitter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_spline_local.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_spline_parametrization.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_spline_pleasing.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_spline_polynomial.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_symbol.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_system_clock.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_text.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_text_engine.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_text_label.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_thermo.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_transform.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_vectorfield_symbol.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_weeding_curve_fitter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_wheel.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_widget_overlay.cpp
  )  

set(BUILD_STATIC TRUE)
set(LOCAL_LIBS ${JVX_QT_LIBS})
include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)

