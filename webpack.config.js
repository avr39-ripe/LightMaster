const path = require('path');
const webpack = require('webpack');
const BabiliPlugin = require("babili-webpack-plugin");
const HtmlWebpackPlugin = require('html-webpack-plugin');

module.exports = {
	resolve: {
		modules: [path.resolve(__dirname, "lib/application"), path.resolve(__dirname, "lib/binio"), path.resolve(__dirname, "lib/tempsensor"), "node_modules"]
	},
	entry : {
		index: './files/index_new.js'
	},
	output : {
		path: path.join(__dirname, 'files'),
		filename : '[name].[hash:10].js'
	},
	module : {
		loaders : [ {
			test : /\.js$/,
			exclude : /(node_modules|bower_components)/,
			loader : 'babel-loader',
			query : {
				presets : [ 'es2015' ]
			}
		} ]
	},
	devtool : 'source-map',
	plugins : [
	//    new webpack.optimize.UglifyJsPlugin({
	//      compress: { warnings: false }
	//    })
	new BabiliPlugin(),
	new HtmlWebpackPlugin({
		template: path.resolve('./', './files/index-template.html'),
		filename: path.resolve('./', './files/index_new.html'),
		inject: 'head'
	    })
	]
}
