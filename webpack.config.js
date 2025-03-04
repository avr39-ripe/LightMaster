const path = require('path');
const HtmlWebpackPlugin = require('html-webpack-plugin');
const InlineChunkHtmlPlugin = require('react-dev-utils/InlineChunkHtmlPlugin');
const CompressionPlugin = require('compression-webpack-plugin');
const TerserPlugin = require('terser-webpack-plugin');

module.exports = {
  mode: process.env.NODE_ENV || 'production',
  entry: {
    index: path.resolve(__dirname, 'web/index.js')
  },
  output: {
    path: path.join(__dirname, 'web/build'),
    filename: '[name].[contenthash:10].js',
    clean: true // Clean the output directory before each build
  },
  resolve: {
    modules: [
      path.resolve(__dirname, "lib/application"),
      path.resolve(__dirname, "lib/binio"),
      path.resolve(__dirname, "lib/tempsensor"),
      path.resolve(__dirname, "files"),
      path.resolve(__dirname, "web"),
      "node_modules"
    ]
  },
  module: {
    rules: [
      {
        test: /\.(js|jsx)$/,
        include: path.resolve(__dirname, 'src'),
        exclude: /node_modules/,
        use: {
          loader: 'babel-loader',
          options: {
            presets: [
              [
                '@babel/preset-env',
                {
                  targets: { browsers: ['last 2 versions'] },
                  modules: false // Needed for tree shaking
                }
              ]
              // Uncomment the next line if you're using React:
              //, '@babel/preset-react'
            ]
          }
        }
      },
      {
        test: /\.css$/,
        use: ['style-loader', 'css-loader'] // CSS is bundled inside JS
      },
      {
        // Inline all assets (images, fonts, SVGs, etc.) as data URIs.
        test: /\.(png|jpe?g|gif|svg|eot|ttf|woff|woff2)$/i,
        type: 'asset/inline'
      }
    ]
  },
  optimization: {
    minimize: true,
    minimizer: [
      new TerserPlugin({
        terserOptions: {
          // Optional Terser configuration can go here.
        }
      })
    ]
  },
  devtool: 'source-map',
  plugins: [
    new HtmlWebpackPlugin({
      template: './web/index-template.html',
      inject: 'head'
    }),
    // Inline all JavaScript chunks into the HTML.
    new InlineChunkHtmlPlugin(HtmlWebpackPlugin, [/\.js$/]),
    // Compress the final HTML file (with inlined assets) into index.html.gz and remove the original.
    new CompressionPlugin({
      test: /\.html$/i,
      filename: '[path][base].gz',
      algorithm: 'gzip',
      deleteOriginalAssets: true
    })
  ]
};
