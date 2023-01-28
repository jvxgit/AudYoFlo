% Plot one segment between x_start and x_stop and y_start and y_stop -
% either filled or only the border
function [] = jvx_plot_one_segment(x_start, x_stop, y_start, y_stop, c, filled)
    hold on;
    y_start = y_start + 1/3 * (y_stop - y_start);
    y_stop = y_stop + 1/3 * (y_start - y_stop);
    if(filled)
        fill([x_start x_stop x_stop x_start]', [y_start y_start y_stop y_stop]', c, 'EdgeColor', c);
    else
        plot([x_start x_stop], ones(1,2)*y_start, c);
        plot([x_start x_stop], ones(1,2)*y_stop, c);
        plot([x_start x_start], [y_start y_stop], c);
        plot([x_stop x_stop], [y_start y_stop], c);
    end
