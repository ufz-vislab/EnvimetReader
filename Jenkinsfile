#!/usr/bin/env groovy
@Library('jenkins-pipeline@1.0.15') _

pipeline {
  agent { label 'win1' }
   environment {
    MSVC_NUMBER = '15'
    MSVC_VERSION = '2017'
  }
  stages {
    stage('Build') {
      steps {
        script {
          configure { cmakeOptions='-DParaView_DIR=E:/bilke/pv/build-pv-5.6.0/superbuild/paraview/build'}
          build { target='all' }
        }
      }
      post {
        success {
          archiveArtifacts '**/*.dll'
          bat 'copy build\\EnvimetReader.dll C:\\paraview\\Plugins-5.6.0'
        }
      }
    }
  }
}
