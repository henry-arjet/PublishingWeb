const path = require('path');
const SRC_DIR = path.join(__dirname, '/react-client/src');
const webpack = require('webpack');


module.exports = {
  entry: `${SRC_DIR}/index.jsx`,
  resolve: {
    extensions: ['.js', '.jsx', '.json', '.css']
  },
  module : {
    rules : [
      {
        test: /\.css$/,
        loader: 'style-loader!css-loader'
      },
      {
        test: /\.png$/,
        loader: 'url-loader?limit=100000&minetype=image/png'
      },
      {
        test: /\.jpg/,
        loader: 'file-loader'
      },
      {
        test : /\.jsx?/,
        include : SRC_DIR,
        loader : 'babel-loader',      
        query: {
          presets: ['@babel/react', '@babel/preset-env']
       }
      },
    ]
  },
  plugins: [
    new webpack.DefinePlugin({
      'process.env.NODE_ENV': JSON.stringify('production')
    }),
    new webpack.optimize.ModuleConcatenationPlugin(),
    new webpack.LoaderOptionsPlugin({
			options: {
				loaders: [
					{
						test: require.resolve('tinymce/tinymce'),
						use: [
							'imports-loader?this=>window',
							'exports-loader?window.tinymce',
						]
					},
					{
						test: /tinymce\/(themes|plugins)\//,
						use: [
							'imports-loader?this=>window'
						]
					},
				],
			}
		})
  ]
};