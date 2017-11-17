numred = @(x) x .* (exp(- ((x-650)./20).^2) + 0.05.*exp(-((x-730)./40).^2));
numgreen = @(x) x .* (0.6 .* exp(- ((x-580)./20).^2) + 0.7.*exp(-((x-600)./10).^2));
numblue = @(x) x .* (0.95 .* exp(- ((x-490)./15).^2));

denomred = @(x) exp(- ((x-650)/20).^2) + 0.05*exp(-((x-730)/40).^-2);
denomgreen = @(x) 0.6 * exp(- ((x-580)/20).^2) + 0.7*exp(-((x-600)/10).^2);
denomblue = @(x) 0.95 * exp(- ((x-490)/15).^2);


avgred = integral(numred, 400, 950) / integral(denomred, 400, 950)
avggrn = integral(green, 400, 950) / integral(denomgreen, 400, 950)
avgblu = integral(blue, 400, 950) / integral(denomblue, 400, 950)